// Copyright by 2022.4 chime
// author: yatorho

#include "chime/core/memory/memory_pool.hpp"

#include "chime/core/framework/common.hpp"

namespace chime {
namespace memory {

utens_t MemoryBlock::blocks_count = 0ul;

ChimeMemoryPool::ChimeMemoryPool(PoolType pType, mems_t size)
    : _pool_size(size),
      _cpu_memory_block(nullptr),
      _cpu_free_mb_list(nullptr),
      _cpu_head(nullptr),
      _gpu_memory_block(nullptr),
      _gpu_free_mb_list(nullptr),
      _gpu_head(nullptr),
      _p_type(pType),
      _p_status(UNINITIALIZED) {}

mems_t ChimeMemoryPool::pool_size() const { return _pool_size; }

PoolType ChimeMemoryPool::pool_type() const { return _p_type; }

ChimeMemoryPool::~ChimeMemoryPool() { destroy(); }

void ChimeMemoryPool::destroy() {
  switch (_p_type) {
    case PoolType::CPU_MEMORY_TYPE:
      switch (_p_status) {
        case UNINITIALIZED: break;
        case WORKING:
          mb_ptr memory_block_ptr;
          DCHECK(_cpu_memory_block);
          memory_block_ptr = _cpu_memory_block;
          if (!_cpu_memory_block->rear)
            LOG(WARNING) << "Destroying the memory pool may result in some "
                            "pointers pointing to freed memory.";
          DCHECK(_cpu_head);
          DCHECK_EQ(_cpu_memory_block->block_status, MemoryBlock::FREE);
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
          std::free(_cpu_head);
          break;
        case READY_TO_BE_FREED:
          delete _cpu_memory_block;
          delete _cpu_free_mb_list;
          std::free(_cpu_head);
      }
      break;
    case PoolType::GPU_MEMORY_TYPE: NOT_IMPLEMENTED; break;
    case PoolType::CPU_AND_GPU_MEMORY_TYPE: NOT_IMPLEMENTED; break;
    default: LOG(FATAL) << "Unknown memory pool's type: " << _p_type;
  }
}

void ChimeMemoryPool::init() {
  switch (_p_type) {
    case PoolType::CPU_MEMORY_TYPE:
      DCHECK(!_cpu_head) << "init function can only be called onced.";
      _cpu_head = std::malloc(_pool_size);

      DCHECK(!_cpu_memory_block);
      _cpu_memory_block = new MemoryBlock;
      _cpu_memory_block->front = nullptr;
      _cpu_memory_block->rear = nullptr;
      _cpu_memory_block->memory = _cpu_head;
      _cpu_memory_block->block_status = MemoryBlock::FREE;
      _cpu_memory_block->size = _pool_size;

      DCHECK(!_cpu_free_mb_list);
      _cpu_free_mb_list = new MemoryBlock;
      _cpu_free_mb_list->front = nullptr;
      _cpu_free_mb_list->rear = nullptr;
      _cpu_free_mb_list->memory = _cpu_head;
      _cpu_free_mb_list->block_status = MemoryBlock::FREE;
      _cpu_free_mb_list->size = _pool_size;
      break;

    case PoolType::GPU_MEMORY_TYPE: NOT_IMPLEMENTED; break;
    case PoolType::CPU_AND_GPU_MEMORY_TYPE: NOT_IMPLEMENTED; break;
    default: LOG(FATAL) << "Unknown memory pool's type";
  }
}

} // namespace memory

} // namespace chime
