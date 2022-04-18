// Copyright by 2022.4 chime
// author: yatorho

#include "chime/core/memory/memory_pool.hpp"

#include "chime/core/framework/common.hpp"

namespace chime {
namespace memory {

ChimeMemoryPool::ChimeMemoryPool(PoolType pType, mems_t size)
    : _pool_size(size),
      _cpu_memory_block(nullptr),
      _cpu_head(nullptr),
      _gpu_memory_block(nullptr),
      _gpu_head(nullptr),
      _p_type(pType),
      _p_status(UNINITIALIZED) {}

mems_t ChimeMemoryPool::pool_size() const { return _pool_size; }

PoolType ChimeMemoryPool::pool_type() const { return _p_type; }

ChimeMemoryPool::PoolStatus ChimeMemoryPool::pool_status() const {
  return _p_status;
}

ChimeMemoryPool::~ChimeMemoryPool() { destroy(); }

void ChimeMemoryPool::destroy() {
  delete_mutex(_mutex);
  switch (_p_status) {
    case UNINITIALIZED: break;
    case WORKING: {
      LOG(WARNING) << "Destroying the memory pool in working status may "
                      "result in some "
                      "pointers pointing to freed memory.";
      switch (_p_type) {
        case PoolType::CPU_MEMORY_TYPE: {
          mb_ptr memory_block_ptr = _cpu_memory_block;
          DCHECK(_cpu_head);
          DCHECK(_cpu_memory_block);

          while (true) {
            memory_block_ptr->front = nullptr;
            memory_block_ptr->front_free = nullptr;
            memory_block_ptr->rear_free = nullptr;
            memory_block_ptr->memory = nullptr;
            if (memory_block_ptr->rear) {
              memory_block_ptr = memory_block_ptr->rear;
              memory_block_ptr->front->rear = nullptr;
              delete memory_block_ptr->front;
            } else
              break;
          }
          std::free(_cpu_head);
          break;
        }
        case PoolType::GPU_MEMORY_TYPE: {
          mb_ptr memory_block_ptr = _cpu_memory_block;
          DCHECK(_gpu_head);
          DCHECK(_gpu_memory_block);

          while (true) {
            memory_block_ptr->front = nullptr;
            memory_block_ptr->front_free = nullptr;
            memory_block_ptr->rear_free = nullptr;
            memory_block_ptr->memory = nullptr;
            if (memory_block_ptr->rear) {
              memory_block_ptr = memory_block_ptr->rear;
              memory_block_ptr->front->rear = nullptr;
              delete memory_block_ptr->front;
            } else
              break;
          }
          std::free(_gpu_head);
          break;
        }
        case PoolType::CPU_AND_GPU_MEMORY_TYPE: {
          mb_ptr memory_block_ptr;
          memory_block_ptr = _cpu_memory_block;
          DCHECK(_cpu_head);
          DCHECK(_cpu_memory_block);

          while (true) {
            memory_block_ptr->front = nullptr;
            memory_block_ptr->front_free = nullptr;
            memory_block_ptr->rear_free = nullptr;
            memory_block_ptr->memory = nullptr;
            if (memory_block_ptr->rear) {
              memory_block_ptr = memory_block_ptr->rear;
              memory_block_ptr->front->rear = nullptr;
              delete memory_block_ptr->front;
            } else
              break;
          }
          std::free(_cpu_head);

          memory_block_ptr = _gpu_memory_block;
          DCHECK(_gpu_head);
          DCHECK(_gpu_memory_block);

          while (true) {
            memory_block_ptr->front = nullptr;
            memory_block_ptr->front_free = nullptr;
            memory_block_ptr->rear_free = nullptr;
            memory_block_ptr->memory = nullptr;
            if (memory_block_ptr->rear) {
              memory_block_ptr = memory_block_ptr->rear;
              memory_block_ptr->front->rear = nullptr;
              delete memory_block_ptr->front;
            } else
              break;
          }
          std::free(_gpu_head);
          break;
        }
      }
      break;
    }
    case READY_TO_BE_FREED: {
      switch (_p_type) {
        case PoolType::CPU_MEMORY_TYPE:
          delete _cpu_memory_block;
          std::free(_cpu_head);
          break;
        case PoolType::GPU_MEMORY_TYPE:
          delete _gpu_memory_block;
          std::free(_gpu_memory_block);
        case PoolType::CPU_AND_GPU_MEMORY_TYPE:
          delete _cpu_memory_block;
          std::free(_cpu_head);
          delete _gpu_memory_block;
          std::free(_gpu_head);
          break;
      }
      break;
    }
  }
  _p_status = UNINITIALIZED;
}

void ChimeMemoryPool::init() {
  init_mutex(_mutex);
  switch (_p_type) {
    case PoolType::CPU_MEMORY_TYPE:
      switch (_p_status) {
        case UNINITIALIZED:
          DCHECK(!_cpu_head);
          _cpu_head = std::malloc(_pool_size);

          DCHECK(!_cpu_memory_block);
          _cpu_memory_block = new MemoryBlock;
          _cpu_memory_block->front = nullptr;
          _cpu_memory_block->rear = nullptr;
          _cpu_memory_block->front_free = nullptr;
          _cpu_memory_block->rear_free = nullptr;
          _cpu_memory_block->memory = _cpu_head;
          _cpu_memory_block->block_status = MemoryBlock::FREE;
          _cpu_memory_block->size = _pool_size;

          _p_status = READY_TO_BE_FREED;
          break;
        case WORKING:
          LOG(FATAL) << "init function can onlye be called onced.";
          break;
        case READY_TO_BE_FREED:
          LOG(FATAL) << "init function can onlye be called onced.";
          break;
      }
      break;
    case PoolType::GPU_MEMORY_TYPE: NOT_IMPLEMENTED; break;
    case PoolType::CPU_AND_GPU_MEMORY_TYPE: NOT_IMPLEMENTED; break;
    default: LOG(FATAL) << "Unknown memory pool's type";
  }
}

void ChimeMemoryPool::malloc(void **ptr, mems_t size,
                             MallocDeviceType malloc_type) {
  lock(_mutex);
  switch (malloc_type) {
    case MALLOC_FROM_CPU_MEMORY: {
      DCHECK_NE(_p_status, UNINITIALIZED)
        << "memory pool hasn't been initialized.";
      DCHECK_NE(_p_type, PoolType::GPU_MEMORY_TYPE);

      mb_ptr block;
      _cpu_memory_block->block_status == MemoryBlock::FREE
        ? block = _cpu_memory_block
        : block = _cpu_memory_block->rear_free;

      while (true) {
        DCHECK(block) << "couldn't allocate " << size
                      << " bits memory from cpu device.";
        if (size <= block->size) {
          if (size == block->size) {
            block->front->rear_free = block->rear_free;
            block->front_free->rear_free = block->rear_free;

            block->rear->front_free = block->front_free;
            block->rear_free->front_free = block->front_free;
          } else {
            mb_ptr new_block = new MemoryBlock;

            new_block->block_status = MemoryBlock::FREE;
            new_block->size = block->size - size;
            new_block->front = block;
            new_block->rear = block->rear;
            new_block->front_free = block->front_free;
            new_block->rear_free = block->rear_free;
            new_block->memory =
              static_cast<void *>(((char *) block->memory) + size);

            if (block->front) block->front->rear_free = new_block;
            if (block->front_free) block->front_free->rear_free = new_block;

            if (block->rear) {
              block->rear->front = new_block;
              block->rear->front_free = new_block;
            }
            if (block->rear_free) block->rear_free->front_free = new_block;

            block->rear = new_block;
            block->rear_free = new_block;
            block->size = size;
          }
          *ptr = block->memory;
          block->block_status = MemoryBlock::OCCUPIED;

          _p_status = ChimeMemoryPool::WORKING;
          break;
        } else {
          block = block->rear_free;
        }
      }
      break;
    }
    case MALLOC_FROM_GPU_MEMORY: {
      DCHECK_NE(_p_status, UNINITIALIZED)
        << "memory pool hasn't been initialized.";
      DCHECK_NE(_p_type, PoolType::CPU_MEMORY_TYPE);
      NOT_IMPLEMENTED;
      break;
    }
  }
  unlock(_mutex);
}

void ChimeMemoryPool::malloc(void **ptr, mems_t size) {
  switch (_p_type) {
    case PoolType::CPU_MEMORY_TYPE:
      malloc(ptr, size, MALLOC_FROM_CPU_MEMORY);
      break;
    case PoolType::GPU_MEMORY_TYPE:
      malloc(ptr, size, MALLOC_FROM_GPU_MEMORY);
      break;
    case PoolType::CPU_AND_GPU_MEMORY_TYPE:
      LOG(FATAL) << "MallocDeviceType should be explicited with cpu and gpu "
                    "memory pool.";
      break;
  }
}

} // namespace memory

} // namespace chime
