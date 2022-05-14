// Copyright by 2022.4 chime
// author: yatorho

#include "chime/core/memory/pool.hpp"
namespace chime {
namespace memory {

ChimeMemoryPool::ChimeMemoryPool(PoolType pType, mems_t size)
    : _pool_size(size),
      _cpu_memory_block(nullptr),
      _cpu_head(nullptr),
      _cpu_next_free(nullptr),
      _cpu_next_malloc(nullptr),
      _gpu_memory_block(nullptr),
      _gpu_head(nullptr),
      _gpu_next_free(nullptr),
      _gpu_next_malloc(nullptr),
      _p_type(pType),
      _p_status(UNINITIALIZED) {}

mems_t ChimeMemoryPool::pool_size() const { return _pool_size; }

ChimeMemoryPool::PoolType ChimeMemoryPool::pool_type() const { return _p_type; }

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
                      "result in some pointers pointing to freed memory.";
      switch (_p_type) {
        case PoolType::CPU_MEMORY_TYPE: {
          mb_ptr memory_block_ptr = _cpu_memory_block;
          DCHECK(_cpu_head);
          DCHECK(_cpu_memory_block);

          while (true) {
            memory_block_ptr->front = nullptr;
            memory_block_ptr->memory = nullptr;
            if (memory_block_ptr->rear) {
              memory_block_ptr = memory_block_ptr->rear;
              memory_block_ptr->front->rear = nullptr;
              delete memory_block_ptr->front;
            } else
              break;
          }

          _cpu_memory_block = nullptr;
          std::free(_cpu_head);
          _cpu_head = nullptr;
          break;
        }
        case PoolType::GPU_MEMORY_TYPE: {
          mb_ptr memory_block_ptr = _gpu_memory_block;
          DCHECK(_gpu_head);
          DCHECK(_gpu_memory_block);

          while (true) {
            memory_block_ptr->front = nullptr;
            memory_block_ptr->memory = nullptr;
            if (memory_block_ptr->rear) {
              memory_block_ptr = memory_block_ptr->rear;
              memory_block_ptr->front->rear = nullptr;
              delete memory_block_ptr->front;
            } else
              break;
          }
          _gpu_memory_block = nullptr;
          std::free(_gpu_head);
          _gpu_head = nullptr;
          break;
        }
        case PoolType::CPU_AND_GPU_MEMORY_TYPE: {
          mb_ptr memory_block_ptr;
          memory_block_ptr = _cpu_memory_block;
          DCHECK(_cpu_head);
          DCHECK(_cpu_memory_block);

          while (true) {
            memory_block_ptr->front = nullptr;
            memory_block_ptr->memory = nullptr;
            if (memory_block_ptr->rear) {
              memory_block_ptr = memory_block_ptr->rear;
              memory_block_ptr->front->rear = nullptr;
              delete memory_block_ptr->front;
            } else
              break;
          }
          _cpu_memory_block = nullptr;
          std::free(_cpu_head);
          _cpu_head = nullptr;

          memory_block_ptr = _gpu_memory_block;
          DCHECK(_gpu_head);
          DCHECK(_gpu_memory_block);

          while (true) {
            memory_block_ptr->front = nullptr;
            memory_block_ptr->memory = nullptr;
            if (memory_block_ptr->rear) {
              memory_block_ptr = memory_block_ptr->rear;
              memory_block_ptr->front->rear = nullptr;
              delete memory_block_ptr->front;
            } else
              break;
          }
          _gpu_memory_block = nullptr;
          std::free(_gpu_head);
          _gpu_head = nullptr;
          break;
        }
      }
      break;
    }
    case READY_TO_BE_FREED: {
      switch (_p_type) {
        case PoolType::CPU_MEMORY_TYPE:
          delete _cpu_memory_block;
          _cpu_memory_block = nullptr;
          _cpu_next_malloc = nullptr;
          DCHECK(!_cpu_next_free);
          std::free(_cpu_head);
          _cpu_head = nullptr;
          break;
        case PoolType::GPU_MEMORY_TYPE:
          delete _gpu_memory_block;
          _gpu_memory_block = nullptr;
          _gpu_next_malloc = nullptr;
          DCHECK(!_gpu_next_free);
          std::free(_gpu_memory_block);
          _gpu_head = nullptr;
        case PoolType::CPU_AND_GPU_MEMORY_TYPE:
          delete _cpu_memory_block;
          _cpu_memory_block = nullptr;
          _cpu_next_malloc = nullptr;
          DCHECK(!_cpu_next_free);
          std::free(_cpu_head);
          _cpu_head = nullptr;
          delete _gpu_memory_block;
          _gpu_memory_block = nullptr;
          _gpu_next_malloc = nullptr;
          DCHECK(!_gpu_next_free);
          std::free(_gpu_head);
          _gpu_head = nullptr;
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
          _cpu_memory_block->memory = _cpu_head;
          _cpu_memory_block->block_status = MemoryBlock::FREE;
          _cpu_memory_block->size = _pool_size;

          DCHECK(!_cpu_next_free);
          DCHECK(!_cpu_next_malloc);
          _cpu_next_malloc = _cpu_memory_block;

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

inline mb_ptr find_rear_free_block(mb_ptr block) {
  if (!block) return nullptr;
  return block->block_status == MemoryBlock::FREE
    ? block
    : find_rear_free_block(block->rear);
}

void ChimeMemoryPool::malloc(void **ptr, mems_t size, MallocType type) {
  lock(_mutex);
  switch (type) {
    case MALLOC_FROM_HOST_MEMORY: {
      DCHECK_NE(_p_status, UNINITIALIZED)
        << "memory pool hasn't been initialized.";
      DCHECK_NE(_p_type, PoolType::GPU_MEMORY_TYPE);

      mb_ptr block = _cpu_next_malloc;

      while (true) {
        DCHECK(block) << "couldn't allocate " << size
                      << " bits memory from cpu device.";
        if (size <= block->size) {
          if (size < block->size) {
            mb_ptr new_block = new MemoryBlock;

            new_block->block_status = MemoryBlock::FREE;
            new_block->size = block->size - size;
            new_block->front = block;
            new_block->rear = block->rear;
            new_block->memory =
              static_cast<void *>(((char *) block->memory) + size);

            if (block->rear) block->rear->front = new_block;

            block->rear = new_block;
            block->size = size;
          }
          *ptr = block->memory;
          block->block_status = MemoryBlock::OCCUPIED;

          if (!_cpu_next_free) _cpu_next_free = block;
          DCHECK(_cpu_next_free);
          if ((char *) (_cpu_next_free->memory) < (char *) (block->memory)) {
            _cpu_next_free = block;
          }

          _p_status = ChimeMemoryPool::WORKING;
          break;
        } else {
          block = find_rear_free_block(block->rear);
        }
      }
      _cpu_next_malloc = find_rear_free_block(_cpu_next_malloc);
      break;
    }
    case MALLOC_FROM_DEVICE0_MEMORY: {
      DCHECK_NE(_p_status, UNINITIALIZED)
        << "memory pool hasn't been initialized.";
      DCHECK_NE(_p_type, PoolType::CPU_MEMORY_TYPE);
      NOT_IMPLEMENTED;
      break;
    }
    default: LOG(FATAL) << "unknown malloc type!"; break;
  }
  unlock(_mutex);
}

void ChimeMemoryPool::malloc(void **ptr, mems_t size) {
  switch (_p_type) {
    case PoolType::CPU_MEMORY_TYPE:
      malloc(ptr, size, MALLOC_FROM_HOST_MEMORY);
      break;
    case PoolType::GPU_MEMORY_TYPE:
      malloc(ptr, size, MALLOC_FROM_DEVICE0_MEMORY);
      break;
    case PoolType::CPU_AND_GPU_MEMORY_TYPE:
      LOG(FATAL)
        << "MallocDeviceType should be explicated for cpu and gpu memory pool.";
      break;
  }
}

inline mb_ptr find_front_occupied_block(mb_ptr block) {
  if (!block) return nullptr;
  return block->block_status == MemoryBlock::OCCUPIED
    ? block
    : find_front_occupied_block(block->front);
}

inline mb_ptr combine_block(mb_ptr block) {
  if (block->front && block->rear) {
    if (block->front->block_status == MemoryBlock::FREE
        && block->rear->block_status == MemoryBlock::FREE) {
      block->front->size += block->size + block->rear->size;
      block->front->rear = block->rear->rear;
      if (block->rear->rear) block->rear->rear->front = block->front;
      delete block->rear;
      delete block;
      return block->front;
    }
    if (block->front->block_status == MemoryBlock::FREE
        && block->rear->block_status == MemoryBlock::OCCUPIED) {
      block->front->size += block->size;
      block->front->rear = block->rear;
      block->rear->front = block->front;
      delete block;
      return block->front;
    }
    if (block->front->block_status == MemoryBlock::OCCUPIED
        && block->rear->block_status == MemoryBlock::FREE) {
      block->size += block->rear->size;
      delete block->rear;
      block->rear = block->rear->rear;
      if (block->rear) { block->rear->front = block; }
      return block;
    }
  }
  if (block->front) {
    if (block->front->block_status == MemoryBlock::FREE) {
      block->front->size += block->size;
      block->front->rear = nullptr;
      delete block;
      return block->front;
    }
  }
  if (block->rear) {
    if (block->rear->block_status == MemoryBlock::FREE) {
      block->size += block->rear->size;
      delete block->rear;
      block->rear = block->rear->rear;
      if (block->rear) block->rear->front = block;
      return block;
    }
  }
  return block;
}

void ChimeMemoryPool::free(void *ptr, FreeType type) {
  lock(_mutex);
  switch (type) {
    case FREE_FROM_HOST_MEMORY: {
      DCHECK_NE(_p_type, PoolType::GPU_MEMORY_TYPE);
      DCHECK_EQ(_p_status, WORKING) << "memory pool in " << _p_status
                                    << " status couldn't called free function.";

      mb_ptr block = _cpu_next_free;
      while (true) {
        DCHECK(block) << "failed to locate pointer: " << ptr;
        if (block->memory == ptr) {
          DCHECK_EQ(block->block_status, MemoryBlock::OCCUPIED);

          block->block_status = MemoryBlock::FREE;
          _cpu_next_free = find_front_occupied_block(_cpu_next_free);
          mb_ptr free_block = combine_block(block);
          DCHECK(free_block);
          if (!_cpu_next_malloc) _cpu_next_malloc = free_block;
          if ((char *) (_cpu_next_malloc->memory)
              > (char *) (free_block->memory))
            _cpu_next_malloc = free_block;
          break;
        } else {
          block = find_front_occupied_block(block->front);
        }
      }
      if (!_cpu_next_free) _p_status = READY_TO_BE_FREED;
      break;
    }
    case FREE_FROM_DEVICE0_MEMORY: {
      NOT_IMPLEMENTED;
      break;
    }
    default: LOG(FATAL) << "unknown free type!"; break;
  }
  unlock(_mutex);void memcpy(void *dst, void *src, const mems_t size, CopyType type);
}

void ChimeMemoryPool::free(void *ptr) {
  switch (_p_type) {
    case PoolType::CPU_MEMORY_TYPE: free(ptr, FREE_FROM_HOST_MEMORY); break;
    case PoolType::GPU_MEMORY_TYPE: free(ptr, FREE_FROM_DEVICE0_MEMORY); break;
    case PoolType::CPU_AND_GPU_MEMORY_TYPE:
      LOG(FATAL)
        << "FreeDeviceType should be explicated for cpu and gpu memory pool.";
      break;
  }
}

void ChimeMemoryPool::memcpy(void *dst, const void *src, mems_t size,
                             MemoryOptimizer::CopyType type) {
  switch (_p_type) {
    case PoolType::CPU_MEMORY_TYPE: {
      std::memcpy(dst, src, size);
      break;
    }
    case PoolType::GPU_MEMORY_TYPE: NOT_IMPLEMENTED;
    case PoolType::CPU_AND_GPU_MEMORY_TYPE: NOT_IMPLEMENTED;
  }
}
} // namespace memory
} // namespace chime