#pragma once

#ifndef ImGINE_JOBSYSTEM
#define ImGINE_JOBSYSTEM


/// highly inspired from https://github.com/hlavacs/ViennaGameJobSystem



#include <iostream>
#include <fstream>
#include <cstdint>
#include <atomic>
#include <mutex>
#include <thread>
#include <future>
#include <functional>
#include <condition_variable>
#include <queue>
#include <map>
#include <set>
#include <iterator>
#include <algorithm>
#include <assert.h>
#include <type_traits>
#include <chrono>
#include <string>
#include <sstream>
#include <compare>
#include <unordered_map>
#include <memory_resource>

#include "imgine_inttypes.h"

using pfvoid = void(*)();
using thread_index_t = int_type<int, struct P0, -1>;
using thread_id_t = int_type<int, struct P1, -1>;
using thread_type_t = int_type<int, struct P2, -1>;
using thread_count_t = int_type<int, struct P3, -1>;
using tag_t = int_type<int, struct P4, -1>;
using parent_t = int_type<int, struct P5, -1>;

namespace n_pmr = std::pmr;


template<typename>
struct is_pmr_vector : std::false_type {};

template<typename T>
struct is_pmr_vector<n_pmr::vector<T>> : std::true_type {};


class Job;
class JobBase;
class JobSystem;

/// <summary>
/// Wraps c++ function of type void(void)
/// </summary>
struct FunctionWrapper
{
    std::function<void(void)>   function = []() {};  //empty function
    thread_index_t              thread_index;        //thread that the f should run on

    FunctionWrapper(std::function<void(void)>& f, thread_index_t index = thread_index_t{})
        : function(f), thread_index(index) {}
    FunctionWrapper(std::function<void(void)>&& f,thread_index_t index = thread_index_t{})
        : function(std::move(f)), thread_index(index) {}

    FunctionWrapper(const FunctionWrapper& f) = default;
    FunctionWrapper(FunctionWrapper&& f) = default;
    FunctionWrapper& operator= (const FunctionWrapper& f) = default;
    FunctionWrapper& operator= (FunctionWrapper&& f) = default;

    decltype(auto) get_function()& { return function; }
    decltype(auto) get_function()&& { return std::move(function); }
};



template<typename T>
concept FUNCTION = std::is_same_v<std::decay_t<T>, FunctionWrapper >;

template<typename T>
concept STDFUNCTION = std::is_convertible_v< std::decay_t<T>, std::function<void(void)> >;

template<typename T>
concept FUNCTOR = FUNCTION<T> || STDFUNCTION<T>;



/// <summary>
/// Base class for deallocation
/// </summary>
struct job_deallocator
{
    virtual void deallocate(JobBase* job) noexcept;
};

/// <summary>
/// Base class for things in Queue
/// </summary>
class Queuable {
public:
    Queuable* next = nullptr;           //next job in the queue
};


class JobBase : public Queuable
{
public:
    std::atomic<int>    children;               //number of children this job is waiting for
    JobBase*            parent;                 //parent job
    thread_index_t      thread_index;           //thread that the job should run on and ran on
    bool                isFunction;             //Is it a function (default false)

    JobBase() : children{ 0 }, parent{ nullptr }, thread_index{}, isFunction{ false } {}
    virtual bool resume() = 0;                  //actual work to be done
    void operator()() noexcept {
        resume();
    }
    bool is_function() noexcept { return isFunction; }
    virtual job_deallocator get_deallocator() noexcept { return job_deallocator{}; };
};


class Job : public JobBase
{
public:
    n_pmr::memory_resource*  mr = nullptr;              //memory ressource used for job allocation
    JobBase*                    continuation = nullptr; //continuation to be executed after this job
    std::function<void(void)>   function;               // function to execute (can accept std::function or lambda)
    pfvoid                      pfvoid = nullptr;       // function to execute (can accept void(*)() )

    Job(n_pmr::memory_resource* pmr) : JobBase(), mr(pmr), continuation(nullptr) {
        children = 1;
        isFunction = true;
    }

    void reset() noexcept { //used for recycling in queue
        next = nullptr;
        children = 1;
        parent = nullptr;
        continuation = nullptr;
        thread_index = thread_index_t{};
    }

    bool resume() noexcept { //call the funtion
        children = 1; // Myself = 1 => tree_from_me = Myself + childrenS
        if (pfvoid != nullptr) pfvoid();
        else function();
        return true;
    }

    bool deallocate() noexcept { return true; } 

};


inline void job_deallocator::deallocate(JobBase* job) noexcept {
    n_pmr::polymorphic_allocator<Job> allocator(((Job*)job)->mr); //construct a polymorphic allocator
    ((Job*)job)->~Job();                                          //call destructor
    allocator.deallocate(((Job*)job), 1);                         //use pma to deallocate the memory
}


/// <summary>
/// FIFO queue. 
/// 
/// Allows Multiple Producers Multiple Consumers.
/// </summary>

template<typename JOB = Queuable, bool SYNC = true> // Our jobs are all Queuable (SYNC = optinal tread unsafe mode)
requires std::is_base_of_v<Queuable,JOB>
class JobQueue {
    friend JobSystem;
    std::atomic_flag m_lock = ATOMIC_FLAG_INIT;     //Queue lock
    JOB* m_head = nullptr;                            //first entry
    JOB* tail = nullptr;                            //last entry
    int32_t size = 0;                               //nmb of entries
public:
    JobQueue() noexcept : m_head(nullptr), tail(nullptr), size(0) {}
    JobQueue(const JobQueue<JOB>& queue) noexcept : m_head(nullptr), tail(nullptr), size(0) {}


    uint32_t clear() {
        uint32_t res = size;
        JOB* job = pop();                       // get first job to deallocate 
        while (job != nullptr) {                // while exist
            auto da = job->get_deallocator();   // retrieve deallocator
            da.deallocate(job);                    // deallocate
            job = pop();                        // get next one
        }
        return res;
    }

    ~JobQueue() {}

    /// <summary>
    /// Number of jobs currently in queue
    /// </summary>
    /// <returns></returns>
    uint32_t  lenght() {
        if constexpr (SYNC) {                                       //if required safe access to JobQueue
            while (m_lock.test_and_set(std::memory_order::acquire));  //acquire lock 
        }
        auto s = size;
        if constexpr (SYNC) {
            m_lock.clear(std::memory_order::release);                 // and release it
        }
        return s;
    }


    void push(JOB* job)
    {
        if constexpr (SYNC) {
            while (m_lock.test_and_set(std::memory_order::acquire)); //acquire lock
        }

        job->next = nullptr;        //new job have no successor => clear it
        if (m_head == nullptr) {      // queue is empty
            m_head = job;             // head point to the new job
        }
        if (tail == nullptr) {      //queue was empty
            tail = job;             //tail point to the new job
        }
        else {                      //queue was not empty
            tail->next = (Job*)job; //add new job to tail job
            tail = job;             //new job is tail 
        }

        size++;                     // size increased with new job
        if constexpr (SYNC) {
            m_lock.clear(std::memory_order::release);                 // and release it
        }
    }


    JOB* pop() {
        if (m_head == nullptr) return nullptr;    //no job to pop

        if constexpr (SYNC) {
            while (m_lock.test_and_set(std::memory_order::acquire)); //acquire lock
        }



        JOB* head = this->m_head;
        if (head != nullptr) {              //if there is a job at the head of the queue
            this->m_head = (JOB*)head->next;    //let point m_head to its successor
            size--;                       //decrease number of jobs
            if (head == tail) {           //if this is the only job
                tail = nullptr;           //let m_tail point to nullptr
            }
        }
        if constexpr (SYNC) {
            m_lock.clear(std::memory_order::release);                 // and release it
        }
        return head;
    }
};




/// <summary>
/// Main JobSystem class for all job system
/// 
/// Start N threads and provide them with data structures
/// Can add new jobs and wait until done
/// </summary>
class JobSystem {
    static inline const uint32_t queue_capacity = 1 << 10; ///<save at most N Jobs for recycling
    static inline const bool enable_logging = false;
private:
    static inline std::atomic<uint64_t>                                 init_counter = 0;                   //
    static inline n_pmr::memory_resource*                               mr;                                 // deallocate/allocate jobs
    static inline std::vector<std::thread>                              threads;                            // array of thread
    static inline std::atomic<uint32_t>                                 thread_count = 0;                   // thread count
    static inline std::atomic<bool>                                     m_terminated = false;                 // flag when last thread exited
    static inline thread_local thread_index_t                           start_idx;                          // idx of first created thread
    static inline thread_local thread_index_t                           thread_index = thread_index_t{};     // each thread has its own number
    static inline std::atomic<bool>                                     m_terminate = false;                  // flag for terminating the pool
    static inline thread_local JobBase*                                 active_job = nullptr;              // Pointer to the current job of this thread
    static inline std::vector<JobQueue<JobBase>>                        global_queues;                      // each thread have it's shared queue   (Multiple produce multiple consume)           
    static inline std::vector<JobQueue<JobBase>>                        local_queues;                       // each thread have it's own local queue  (Multiple produce Single consume)    
    static inline std::vector<std::unique_ptr<std::condition_variable>> cv;                                 // Each thread his own condition variable 
    static inline std::vector<std::unique_ptr<std::mutex>>              mutex;                              // Each thread his own mutex
    static inline std::unordered_map<tag_t, std::unique_ptr<JobQueue<JobBase>>, tag_t::hash> tag_queues;    // Jobs can be attributed to a tag which will only schedule them when all previous required tag have ended
    static inline thread_local JobQueue<Job, false>                     recycled;                           // Old jobs waiting for a new task to be refilled
    static inline thread_local JobQueue<Job, false>                     deleted;                            // Old jobs waiting to be deleted


    /**
        * \brief Allocate a job so that it can be scheduled.
        *
        * If there is a job in the recycle queue we use this. Else a new
        * new Job struct is allocated from the memory resource m_mr.
        *
        * \returns a pointer to the job.
        */
    Job* allocate_job() {
        Job* job = recycled.pop();                                 //try recycle queue
        if (job == nullptr) {                                      //none found
            n_pmr::polymorphic_allocator<Job> allocator(mr);      //use this allocator
            job = allocator.allocate(1);                            //allocate the object
            if (job == nullptr) {
                std::cout << "No job available\n";
                std::terminate();
            }
            new (job) Job(mr);                 //call constructor
        }
        else {                                  //job found
            job->reset();                       //reset it
        }
        return job;
    }

    /**
    * \brief Allocate a job so that it can be scheduled.
    * \param[in] f Function that should be executed by the job.
    * \returns a pointer to the Job.
    */
    template <typename F>
    requires FUNCTOR<F>
    Job* allocate_job(F&& f) noexcept {
        Job* job = allocate_job();
        if constexpr (std::is_same_v<std::decay_t<F>, FunctionWrapper>) { // if it is a function copy it
            job->function = f.get_function();
            job->pfvoid = nullptr;
            job->thread_index = f.thread_index;
        }
        else {
            if constexpr (std::is_pointer_v<std::remove_reference_t<decltype(f)>>) { //use right variable to store function depending on it's type (void(void)/function)
                job->pfvoid = f;
            }
            else {
                job->function = f; //std::function<void(void)> or a lambda
                job->pfvoid = nullptr;
            }
        }

        if (!job->function && !job->pfvoid) {
            std::cout << "Empty function\n";
            std::terminate();
        }
        return job;
    }

public:


    /**
       * \brief JobSystem class constructor.
       * \param[in] threadCount Number of threads in the system.
       * \param[in] start_idx Number of first thread, if 1 then the main thread should enter as thread 0.
       * \param[in] mr The memory resource to use for allocating Jobs.
       */
    JobSystem(thread_count_t threadCount = thread_count_t(0), thread_index_t start_idx = thread_index_t(0)
        , n_pmr::memory_resource* mr = n_pmr::new_delete_resource()) noexcept {

        if (init_counter > 0) [[likely]] return;
        auto cnt = init_counter.fetch_add(1);
        if (cnt > 0) return;

        this->mr = mr;
        start_idx = start_idx;
        m_terminate = false;
        m_terminated = false;

        thread_count = threadCount.value;
        if (thread_count <= 0) {
            thread_count = std::thread::hardware_concurrency();		///< main thread is also running
        }
        if (thread_count == 0) {
            thread_count = 1;
        }

        for (uint32_t i = 0; i < thread_count; i++) {
            global_queues.push_back(JobQueue<JobBase>());     //global job queue
            local_queues.push_back(JobQueue<JobBase>());     //local job queue
            cv.emplace_back(std::make_unique<std::condition_variable>());
            mutex.emplace_back(std::make_unique<std::mutex>());
        }

        for (uint32_t i = start_idx.value; i < thread_count; i++) {
            //std::cout << "Starting thread " << i << std::endl;
            threads.push_back(std::thread(&JobSystem::thread_task, this, thread_index_t(i)));	//spawn the pool threads
            threads[i].detach();
        }

    }


    /// <summary>
    /// Check if system already started
    /// </summary>
    /// <returns>true if the instance exist, else false</returns>
    static bool is_instance_created() noexcept {
        return active_job != nullptr;
    }


    JobSystem(const JobSystem&) = delete;
    JobSystem& operator=(const JobSystem&) = delete;
    JobSystem(JobSystem&&) = default;
    JobSystem& operator=(JobSystem&&) = default;
    ~JobSystem() = default;



    void on_finished(Job* job) noexcept; //called when job finished = all children finished


    /// <summary>
    /// Child tells its parent it have finished
    /// 
    /// Decrease the number of childs left. If it was the last one then the parent also finishes (wich may call also his own parent etc).
    /// A job is also his own child => it must have done his job and finished before returning.
    /// </summary>
    /// <param name="job"></param>
    /// <returns></returns>
    inline bool child_finished(JobBase* job) noexcept {
        uint32_t num = job->children.fetch_sub(1);              //one less child
        if (num == 1) {                                         // was it the last one
            if (job->is_function()) {
                on_finished((Job*)job);                         //if yes call finish on job
            }
            else {
                schedule_job(job);                              //else schedule it
            }
            return true;
        }
        return false;
    }


    void thread_task(thread_index_t threadIndex = thread_index_t(0)) noexcept {
        constexpr uint32_t NOOP = 1 << 8;                                       // number of empty loops before garbage collector
        thread_local static uint32_t noop_counter = 0;                          
        thread_index = threadIndex;                                             // remember your own thread index number
        static std::atomic<uint32_t> thread_counter = thread_count.load();      // counted down when started

        thread_counter--;                                                       // count down
        while (thread_counter.load() > 0) {}                                    // continue only if all threads are running
        

        uint32_t next = rand() % thread_count;                                  // initialize at random position for stealing
        
        std::unique_lock<std::mutex> lk(*mutex[threadIndex.value]);             

        while (!m_terminate) {                                                   // run until job system is terminated
            active_job = local_queues[threadIndex.value].pop();                // try get job in local queue
            if (active_job == nullptr) {
                active_job = global_queues[thread_index.value].pop();          // try in global if none in local
            }
            int num_try = thread_count - 1;
            while (active_job == nullptr && --num_try > 0) {                   // try to steal from other global queue if none in global
                if (++next >= thread_count) next = 0;
                active_job = global_queues[next].pop();
            }


            if (active_job != nullptr) {                                       // if we finnaly get a job

                auto is_function = active_job->is_function();      

                (*active_job)();                                               // execute it

                if (is_function) {
                    child_finished((Job*)active_job);                          // a job always finishes itself
                }
                noop_counter = 0;
            }
            else if (++ noop_counter > NOOP) [[unlikely]] {                     //if none were found let thread sleep
                deleted.clear();                                                // delete jobs to reclaim memory
                cv[0]->wait_for(lk, std::chrono::microseconds(100));            
                //m_cv[m_thread_index.value]->wait_for(lk, std::chrono::microseconds(100));
                noop_counter = noop_counter / 2;
            }
        }

        global_queues[thread_index.value].clear();
        local_queues[thread_index.value].clear();


        uint32_t num = thread_count.fetch_sub(1);                               // last thread clears recycle and garbage queues
        recycled.clear();
        deleted.clear();
        if (num == 1) {
            m_terminated = true;
        }

    }


    void recycle(Job* job) noexcept {
        if (recycled.lenght() <= queue_capacity) {
            recycled.push(job);
        }
        else {
            deleted.push(job);
        }
    }

    void terminate() noexcept {
        m_terminate = true;
    }

    void wait_for_termination() noexcept {
        while (m_terminated.load() == false) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }


    static JobBase* current_job() noexcept {
        return active_job;
    }

    thread_index_t get_thread_index() {
        return thread_index;
    }


    n_pmr::memory_resource* memory_ressource() {
        return mr;
    }


    /// <summary>
    /// Schedule a job in the job system
    /// </summary>
    /// <param name="job"> job to schedule</param>
    /// <param name="tg"> associated tag</param>
    /// <returns></returns>
    uint32_t schedule_job(JobBase* job, tag_t tg = tag_t{}) noexcept {
        thread_local static thread_index_t thread_index(rand() % thread_count);

        assert(job != nullptr);

        if (tg.value >= 0) {                                                        // tagged sheduling
            if (!tag_queues.contains(tg)) {
                tag_queues[tg] = std::make_unique<JobQueue<JobBase>>();
            }
            tag_queues.at(tg)->push(job);                                           // save for later tag to play done
            return 0;
        }



        if (job->thread_index.value < 0 || job->thread_index.value >= (int)thread_count) {                          //push in global queue
            thread_index.value = (++thread_index.value) >= (decltype(thread_index.value))thread_count ? 0 : thread_index.value;
            global_queues[thread_index].push(job);
            cv[0]->notify_all();                                                                                    //wake up thread
            return 1;
        }                                                                                                           // push in local queue to specific thread
        local_queues[job->thread_index.value].push(job);                                                            //wake up thread
        cv[0]->notify_all();
        return 1;
    }



    /// <summary>
    /// Schedule all jobs from a tag
    /// </summary>
    uint32_t schedule_tag(tag_t& tg, tag_t tg2 = tag_t{}, JobBase* parent = active_job, int32_t children = -1) noexcept {
        if (!tag_queues.contains(tg)) return 0;

        JobQueue<JobBase>* queue = tag_queues[tg].get();
        uint32_t num_jobs = queue->lenght();


        if (parent != nullptr) {
            if (children < 0) children = num_jobs;
            parent->children.fetch_add((int)children);
        }

        uint32_t num = num_jobs;
        int i = 0;
        while (num > 0) {
            JobBase* job = queue->pop();
            if (!job) return i;
            job->parent = parent;
            schedule_job(job, tag_t{});
            --num;
            ++i;
        }
        return i;
    };

    /// <summary>
    /// Schedule a function holding a function into the job system - or a tag
    /// </summary>
    /// <typeparam name="F">external function taht is copied into the schedule job</typeparam>
    /// <param name="function"></param>
    /// <param name="tg">The tag that is scheduled</param>
    /// <param name="parent">Parent of this Job</param>
    /// <param name="children"> number used to increase the number of children of the parent</param>
    /// <returns></returns>
    template<typename F> 
    requires FUNCTOR<F> || std::is_same_v<std::decay_t<F>,tag_t> 
    uint32_t schedule(F&& function, tag_t tg = tag_t{}, JobBase* parent = active_job, int32_t children = -1) noexcept {
        if constexpr (std::is_same_v<std::decay_t<F>, tag_t>) {
            return schedule_tag(function,tg, parent, children);
        }
        else {
            Job* job = allocate_job(std::forward<F>(function));
            job->parent = nullptr;
            if (tg.value < 0) {
                job->parent = parent;
                if (parent != nullptr) {
                    if (children < 0) children = 1;
                    parent->children.fetch_add((int)children);
                }
            }
            return schedule_job(job, tg);
        }
    };


    /// <summary>
    /// Store a continuation for the current Job. Will be scheduled once the current Job finishes
    /// </summary>
    /// <typeparam name="F"></typeparam>
    /// <param name="f">The funciton to be schedule as continuation</param>
    template<typename F> 
    requires FUNCTOR<F>
    void continuation(F&& f) {
        JobBase* current = current_job();
        if (current == nullptr || !current->is_function()) {
            return;
        }
        ((Job*)current)->continuation = allocate_job(std::forward<F>(f));
    };







};


/// <summary>
/// A job holding a function and all its children have finished
/// 
/// This is called when a JOb and its children has finished.
/// If continuation stored in job the shchedule continuation.
/// Also job's parent notified of this new child.
/// Then if there is a parent the parent's child_finished function is called
/// 
/// </summary>
/// <param name="job"></param>
inline void JobSystem::on_finished(Job* job) noexcept {

    if (job->continuation != nullptr) {

        if (job->parent != nullptr) {
            job->parent->children++;
            job->continuation->parent = job->parent;
        }
        schedule_job(job->continuation);
    }

    if (job->parent != nullptr) {
        child_finished((Job*)job->parent);
    }
    recycle(job);
};


/// <summary>
/// Get the current job taht is executed by the system
/// </summary>
/// <returns></returns>
inline JobBase* current_job() {
    return (JobBase*)JobSystem::current_job();
};



/// <summary>
/// Schedule fucntions into the system, T can be a Function, std::funciton or a task<U>
/// 
/// Children parameter used to pre-increase the number of childre to avoid races between more schedules 
/// and previous children finishingand destroying.
/// If tuple of vectors is scheduled, in the first call children is the total number of all children
/// in all vectors combined. After children set to 0 (by caller).
/// When vector is scheduled, children should be set to -1, and setting the numb of children is handled by the funciton itself 
/// </summary>
/// <typeparam name="F"></typeparam>
/// <param name="functions"> A vector of funcitons to schedule</param>
/// <param name="parent"> Parent of this job</param>
/// <param name="children"> Number used to increase the number of children of the parent</param>
/// <returns>Number of scheduled functions</returns>
template<typename F> 
inline uint32_t schedule(F&& functions, tag_t tg = tag_t{}, JobBase* parent = current_job(), int32_t children = -1) noexcept {
    if constexpr (is_pmr_vector<std::decay_t<F>>::value) {
        if (children < 0) {                                         //default ? use vector size
            children = (int)functions.size();
        }
        auto ret = children;
        for (auto&& f : functions) {                                // schedule all elements, use total number of children for the first call, then 0
            if constexpr (std::is_lvalue_reference_v<decltype(functions)>) {
                schedule(f, tg, parent, children);                  //
            }
            else {
                schedule(std::move(f), tg, parent, children);       //
            }
            children = 0;
        }
        return ret;
    }
    else {
        return JobSystem().schedule(std::forward<F>(functions), tg, parent, children);
    }
};


/// <summary>
/// Store a continuation for the current job. 
/// Continuation will be scheduled once the job finishes
/// </summary>
/// <typeparam name="F"></typeparam>
/// <param name="f">A function to schedule as continuation</param>
template<typename F>
inline void continuation(F&& f) noexcept {
    JobSystem().continuation(std::forward<F>(f)); //forward to the job system
};



/**
* \brief Terminate the job system
*/
inline void terminate() {
    JobSystem().terminate();
}

/**
* \brief Wait for the job system to terminate
*/
inline void wait_for_termination() {
    JobSystem().wait_for_termination();
}





















//
//class ThreadPool
//{
//public:
//    ThreadPool(const int size) : busy_threads(size), threads(std::vector<std::thread>(size)), shutdown_requested(false)
//    {
//        for (size_t i = 0; i < size; ++i)
//        {
//            threads[i] = std::thread(ThreadWorker(this));
//        }
//    }
//
//    ~ThreadPool()
//    {
//        Shutdown();
//    }
//
//    ThreadPool(const ThreadPool&) = delete;
//    ThreadPool(ThreadPool&&) = delete;
//
//    ThreadPool& operator=(const ThreadPool&) = delete;
//    ThreadPool& operator=(ThreadPool&&) = delete;
//
//    // Waits until threads finish their current task and shutdowns the pool
//    void Shutdown()
//    {
//        {
//            std::lock_guard<std::mutex> lock(mutex);
//            shutdown_requested = true;
//            condition_variable.notify_all();
//        }
//
//        for (size_t i = 0; i < threads.size(); ++i)
//        {
//            if (threads[i].joinable())
//            {
//                threads[i].join();
//            }
//        }
//    }
//
//    template <typename F, typename... Args>
//    auto AddTask(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
//    {
//        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
//            std::bind(std::forward<F>(f), std::forward<Args>(args)...));
//
//        auto wrapper_func = [task_ptr]() { (*task_ptr)(); };
//        {
//            std::lock_guard<std::mutex> lock(mutex);
//            queue.push(wrapper_func);
//            // Wake up one thread if its waiting
//            condition_variable.notify_one();
//        }
//
//        // Return future from promise
//        return task_ptr->get_future();
//    }
//
//    int QueueSize()
//    {
//        std::unique_lock<std::mutex> lock(mutex);
//        return queue.size();
//    }
//
//private:
//    class ThreadWorker
//    {
//    public:
//        ThreadWorker(ThreadPool* pool) : thread_pool(pool)
//        {
//        }
//
//        void operator()()
//        {
//            std::unique_lock<std::mutex> lock(thread_pool->mutex);
//            while (!thread_pool->shutdown_requested || (thread_pool->shutdown_requested && !thread_pool->queue.empty()))
//            {
//                thread_pool->busy_threads--;
//                thread_pool->condition_variable.wait(lock, [this] {
//                    return this->thread_pool->shutdown_requested || !this->thread_pool->queue.empty();
//                    });
//                thread_pool->busy_threads++;
//
//                if (!this->thread_pool->queue.empty())
//                {
//                    auto func = thread_pool->queue.front();
//                    thread_pool->queue.pop();
//
//                    lock.unlock();
//                    func();
//                    lock.lock();
//                }
//            }
//        }
//
//    private:
//        ThreadPool* thread_pool;
//    };
//
//public:
//    int busy_threads;
//private:
//    mutable std::mutex mutex;
//    std::condition_variable condition_variable;
//
//    std::vector<std::thread> threads;
//    bool shutdown_requested;
//
//    std::queue<std::function<void()>> queue;
//};
//


#endif