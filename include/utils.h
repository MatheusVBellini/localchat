#include <iostream>

#define DEFINE_MESSAGE(level, str)                                             \
  if (DEBUG) {                                                                 \
    std::cerr << #level << " at " << __FILE__ << ":" << __LINE__ << ": "       \
              << str << std::endl;                                             \
  } else {                                                                     \
    std::cerr << #level ": " << str << std::endl;                              \
  }

#ifdef DEBUG
#define debug(str) DEFINE_MESSAGE(debug, str)
#else
#define debug(str)
#endif

#define info(str) DEFINE_MESSAGE(info, str)

#define warn(str) DEFINE_MESSAGE(warning, str)

#define error(str)                                                             \
  DEFINE_MESSAGE(error, str);                                                  \
  std::exit(-1);
