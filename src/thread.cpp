/* Ox: a general-purpose library.
** Copyright (C) 2024-2025  Rivest Osz
** 
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

#include "thread.hpp"

#ifdef OX_DISABLE_THREAD
	#warning "Flag OX_DISABLE_THREAD is set"
#endif

#ifndef OX_DISABLE_THREAD
	#if !defined(OX_DISABLE_CPPTHREAD) && ox_has_include(<thread>) && ox_has_include(<mutex>) && ox_has_include(<new>)
		#define OX_USE_THREAD_STDCPP
	#elif !defined(OX_DISABLE_PTHREAD) && ox_has_include(<pthread.h>)
		#define OX_USE_THREAD_PTHREAD
	#else
		#error "No <thread> nor <pthread.h> support"
	#endif

	#ifdef OX_USE_THREAD_STDCPP
		#include <thread>
		#include <mutex>
		#include <new>
	#elif defined(OX_USE_THREAD_PTHREAD)
		#include <pthread.h>
	#else
		#error "Well, this is awkward..."
	#endif
#endif

namespace Ox {
	Thread::~Thread(void) {
		#ifdef OX_DISABLE_THREAD
			return;
		#elif defined(OX_USE_THREAD_STDCPP)
			std::thread *t = (std::thread *)handle;
			if(t != nullptr) {
				t->join();
				t->~thread();
				t = nullptr;
			}
		#elif defined(OX_USE_THREAD_PTHREAD)
			pthread_t *t = (pthread_t *)handle;
			if(t == nullptr)
				return;

			pthread_cancel(*t);
			handle = nullptr;
		#else
			#error "Well, this is awkward..."
		#endif
	};

	int Thread::init(Ox::Error &err, callable_t f, void *user) {
		if(err != nullptr)
			return -1;

		if(handle != nullptr) {
			err = "You can't a thread again";
			return -1;
		}

		#ifdef OX_DISABLE_THREAD
			(void)err; (void)f; (void)user;
			err = "Flag OX_DISABLE_THREAD is set";
			return -1;
		#elif defined(OX_USE_THREAD_STDCPP)
			std::thread *t = Ox::inhale<std::thread>(err);
			if(t == nullptr)
				return -1;

			// I hate this...
			new (t) std::thread(f, user);

			handle = t;

			return 0;
		#elif defined(OX_USE_THREAD_PTHREAD)
			struct __thread_user_s {
				callable_t f;
				void *user;
			};

			__thread_user_s q;
			q.f = f;
			q.user = user;

			pthread_t t;

			int rc = pthread_create(&t, nullptr, [](void *qp) -> void * {
				__thread_user_s q = *(__thread_user_s *)qp;
				q.f(q.user);
				return nullptr;
			}, &q);

			if(rc != 0) {
				err = "pthread error";	// [TODO] : Improve error message.
				return -1;
			}
			handle = (void *)t;

			pthread_detach(*(pthread_t *)handle);

			return 0;
		#else
			#error "Well, this is awkward..."
		#endif
	};

	void Thread::join(void) {
		#ifdef OX_DISABLE_THREAD
			return;
		#elif defined(OX_USE_THREAD_STDCPP)
			std::thread *t = (std::thread *)handle;
			if(t != nullptr) {
				t->join();
				t->~thread();
				handle = t = nullptr;
			}
		#elif defined(OX_USE_THREAD_PTHREAD)
			pthread_t *t = (pthread_t *)handle;
			if(t == nullptr)
				return;

			pthread_join(*t, nullptr);
			Ox::exhale(t);
			handle = nullptr;
		#else
			#error "Well, this is awkward..."
		#endif
	};

	Ox::pointer_t Thread::get_id(void) {
		#ifdef OX_DISABLE_THREAD
			return 0;
		#elif defined(OX_USE_THREAD_STDCPP)
			std::thread::id t_id = std::this_thread::get_id();
			std::size_t id_hash = std::hash<std::thread::id>{}(t_id);
			return (Ox::pointer_t)id_hash;
		#elif defined(OX_USE_THREAD_PTHREAD)
			return pthread_self();
		#else
			#error "Well, this is awkward..."
		#endif
	};

	int Thread::hint_hardware_concurrency(void) {
		#ifdef OX_DISABLE_THREAD
			return -1;
		#elif defined(OX_USE_THREAD_STDCPP)
			return std::thread::hardware_concurrency();
		#elif defined(OX_USE_THREAD_PTHREAD)
			return 1;	// [TODO] : Well, this sucks.
		#else
			#error "Well, this is awkward..."
		#endif
	};

	Mutex::Mutex(void) {
		#ifdef OX_DISABLE_THREAD
			return;
		#elif defined(OX_USE_THREAD_STDCPP)
			Ox::Error err;

			std::mutex *m = Ox::inhale<std::mutex>(err);
			handle = m;

			if(m == nullptr)
				return;

			// I hate this...
			new (m) std::mutex();
		#elif defined(OX_USE_THREAD_PTHREAD)
			pthread_mutex_init((pthread_mutex_t *)handle, nullptr);
		#else
			#error "Well, this is awkward..."
		#endif
	};

	Mutex::~Mutex(void) {
		#ifdef OX_DISABLE_THREAD
			return;
		#elif defined(OX_USE_THREAD_STDCPP)
			std::mutex *m = (std::mutex *)handle;
			if(m == nullptr)
				return;

			m->~mutex();
		#elif defined(OX_USE_THREAD_PTHREAD)
			pthread_mutex_t *m = (pthread_mutex_t *)handle;
			if(m != nullptr)
				pthread_mutex_destroy(m);
		#else
			#error "Well, this is awkward..."
		#endif
	};

	bool Mutex::try_lock(Ox::Error &err) {
		if(err != nullptr)
			return false;
		if(handle == nullptr) {
			err = "Unitialized Mutex";
			return false;
		}

		#ifdef OX_DISABLE_THREAD
			err = "Flag OX_DISABLE_THREAD is set";
			return false;
		#elif defined(OX_USE_THREAD_STDCPP)
			std::mutex *m = (std::mutex *)handle;
			bool rc = m->try_lock();
			Ox::pointer_t id_self = Thread::get_id();

			if(rc)
				owner = id_self;
			if(rc == false && owner == id_self)
				rc = true;

			return rc;
		#elif defined(OX_USE_THREAD_PTHREAD)
			pthread_mutex_t *m = (pthread_mutex_t *)handle;
			int rc = pthread_mutex_trylock(m);
			Ox::pointer_t id_self = Thread::get_id();

			bool locked = rc == 0;

			if(locked)
				owner = id_self;
			if(locked == false && owner == id_self)
				locked = true;

			return locked;
		#else
			#error "Well, this is awkward..."
		#endif
	};

	int Mutex::lock(Ox::Error &err) {
		if(err != nullptr)
			return -1;
		if(handle == nullptr) {
			err = "Unitialized Mutex";
			return -1;
		}

		#ifdef OX_DISABLE_THREAD
			err = "Flag OX_DISABLE_THREAD is set";
			return -1;
		#elif defined(OX_USE_THREAD_STDCPP)
			Ox::pointer_t id_self = Thread::get_id();
			if(owner != id_self) {
				err = "Not mutex current owner or already unlocked";
				return -1;
			}

			std::mutex *m = (std::mutex *)handle;
			m->lock();
			owner = id_self;

			return 0;
		#elif defined(OX_USE_THREAD_PTHREAD)
			Ox::pointer_t id_self = Thread::get_id();
			if(owner != id_self) {
				err = "Not mutex current owner or already unlocked";
				return -1;
			}

			pthread_mutex_t *m = (pthread_mutex_t *)handle;
			if(pthread_mutex_lock(m) == 0) {
				owner = id_self;
				return 0;
			}

			err = "pthread error";	// [TODO] : Improve error message.
			return -1;
		#else
			#error "Well, this is awkward..."
		#endif
	};

	int Mutex::unlock(Ox::Error &err) {
		if(err != nullptr)
			return -1;
		if(handle == nullptr) {
			err = "Unitialized Mutex";
			return -1;
		}

		#ifdef OX_DISABLE_THREAD
			err = "Flag OX_DISABLE_THREAD is set";
			return -1;
		#elif defined(OX_USE_THREAD_STDCPP)
			Ox::pointer_t id_self = Thread::get_id();
			if(owner != id_self) {
				err = "Not mutex current owner or already unlocked";
				return -1;
			}

			std::mutex *m = (std::mutex *)handle;
			m->unlock();
			owner = 0;

			return 0;
		#elif defined(OX_USE_THREAD_PTHREAD)
			Ox::pointer_t id_self = Thread::get_id();
			if(owner != id_self) {
				err = "Not mutex current owner or already unlocked";
				return -1;
			}

			pthread_mutex_t *m = (pthread_mutex_t *)handle;
			pthread_mutex_unlock(m);
			owner = 0;

			return 0;
		#else
			#error "Well, this is awkward..."
		#endif
	};
};
