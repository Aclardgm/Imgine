#pragma once
#ifndef ImGINE_DEFINES
#define ImGINE_DEFINES

#include <limits>
#include <iostream>

#define FALSE_ID std::numeric_limits<unsigned int>::max()

#ifdef _DEBUG
#define DEBUGVMAALLOC(allocator,allocation,n,s,u)	{\
									std::string imageName; \
									imageName += n;      \
									imageName += "  ";	\
									imageName += s;		\
									imageName += "  ";	\
									imageName += u;		\
									imageName += "\n";		\
									vmaSetAllocationName(allocator, allocation, imageName.c_str()); \
									std::cout << "Allocated : " << imageName.c_str() << "\n";\
									}
#define DEBUGVMADESTROY(allocator,allocation)	{\
									VmaAllocationInfo allocInfo;\
									vmaGetAllocationInfo(allocator, allocation, &allocInfo);\
									std::cout << "Destroyed : " << allocInfo.pName << "\n";\
									}
#else
	#define DEBUGVMAALLOC(allocator,allocation,n,s,u)
	#define DEBUGVMADESTROY(allocator,allocation)
#endif



#endif