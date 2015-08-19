/*
 * Copyright 2010 Rockchip Electronics S.LSI Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __MPP_BUFFER_H__
#define __MPP_BUFFER_H__

#include "rk_type.h"
#include "mpp_err.h"

/*
 * because buffer usage maybe unknown when decoder is not started
 * buffer group may need to set a default group size limit
 */
#define SZ_1K           (1024)
#define SZ_1M           (SZ_1K*SZ_1K)
#define SZ_2M           (SZ_1M*2)
#define SZ_4M           (SZ_1M*4)
#define SZ_8M           (SZ_1M*8)
#define SZ_64M          (SZ_1M*64)
#define SZ_80M          (SZ_1M*80)

typedef void* MppBuffer;
typedef void* MppBufferGroup;

/*
 * mpp buffer pool support two buffer malloc mode:
 *
 * native mode: all buffer are generated by mpp and buffer type can be read from MppBuffer
 *              under this mode, buffer pool is maintained internally
 *
 *              typical call flow:
 *
 *              mpp_buffer_group_get()  return A
 *              mpp_buffer_get(A)       return a    ref +1 -> used
 *              mpp_buffer_inc_ref(a)               ref +1
 *              mpp_buffer_put(a)                   ref -1
 *              mpp_buffer_put(a)                   ref -1 -> unused
 *              mpp_buffer_group_put(A)
 *
 * commit mode: all buffer are commited out of mpp
 *              under this mode, buffer pool is controlled by external api
 *
 *              typical call flow:
 *
 *              ==== external allocator ====
 *              mpp_buffer_group_get()  return A
 *              mpp_buffer_commit(A, x)
 *              mpp_buffer_commit(A, y)
 *
 *              ======= internal user ======
 *              mpp_buffer_get(A)       return a
 *              mpp_buffer_get(A)       return b
 *              mpp_buffer_put(a)
 *              mpp_buffer_put(b)
 *
 *              ==== external allocator ====
 *              mpp_buffer_group_put(A)
 *
 *              NOTE: commit interface required group handle to record group information
 */
typedef enum {
    MPP_BUFFER_MODE_NATIVE,
    MPP_BUFFER_MODE_COMMIT,
} MppBufferMode;

/*
 * mpp buffer has two types:
 *
 * normal   : normal malloc buffer for unit test or hardware simulation
 * ion      : use ion device under Android/Linux, MppBuffer will encapsulte ion file handle
 */
typedef enum {
    MPP_BUFFER_TYPE_NORMAL,
    MPP_BUFFER_TYPE_ION,
    MPP_BUFFER_TYPE_BUTT,
} MppBufferType;

typedef union  MppBufferData_t          MppBufferData;
union  MppBufferData_t {
    void    *ptr;
    RK_S32  fd;
};

typedef struct {
    MppBufferType   type;
    size_t          size;
    MppBufferData   data;
} MppBufferCommit;

#define BUFFER_GROUP_SIZE_DEFAULT   (SZ_1M*80)

#define mpp_buffer_get(...)         mpp_buffer_get_with_tag(MODULE_TAG, ## __VA_ARGS__)
#define mpp_buffer_group_get(...)   mpp_buffer_group_get_with_tag(MODULE_TAG, ## __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

/*
 * MppBuffer interface
 * these interface will change value of group and buffer so before calling functions
 * parameter need to be checked.
 */
MPP_RET mpp_buffer_commit(MppBufferGroup group, MppBufferCommit *info);
MPP_RET mpp_buffer_get_with_tag(const char *tag, MppBufferGroup group, MppBuffer *buffer, size_t size);
MPP_RET mpp_buffer_put(MppBuffer *buffer);
MPP_RET mpp_buffer_inc_ref(MppBuffer buffer);

MPP_RET mpp_buffer_group_get_with_tag(const char *tag, MppBufferGroup *group, MppBufferType type);
MPP_RET mpp_buffer_group_put(MppBufferGroup *group);
MPP_RET mpp_buffer_group_set_limit(MppBufferGroup group, size_t limit);

#ifdef __cplusplus
}
#endif

#endif /*__MPP_BUFFER_H__*/