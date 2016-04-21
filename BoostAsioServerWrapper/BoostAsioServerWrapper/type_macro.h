#ifndef type_macro_h__
#define type_macro_h__

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

typedef boost::shared_lock<boost::shared_mutex> READ_LOCK;
typedef boost::unique_lock<boost::shared_mutex> WRITE_LOCK;

#endif

