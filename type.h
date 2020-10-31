struct cacheBlock{
  bool valid;
  u_int64_t tag;
};


typedef enum{
  HIT,
  MISS
} cacheStatus;

struct cacheStats {
    u_int64_t reads;
    u_int64_t writes;
    u_int64_t readMisses;
    u_int64_t writeMisses;
};

struct config{
    u_int64_t level;
    u_int64_t blockSize;
    u_int64_t  setAssc;
    u_int64_t  totalSize;
    u_int64_t setCount;
    u_int64_t tagLen;
    u_int64_t indexLen;
    u_int64_t blockLen;
};
