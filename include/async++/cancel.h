// Copyright (c) 2013 Amanieu d'Antras
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef ASYNCXX_H_
# error "Do not include this header directly, include <async++.h> instead."
#endif

namespace async {

// A flag which can be used to request cancellation
class cancellation_token {
	std::atomic<bool> state;

	// Non-copyable and non-movable
	cancellation_token(const cancellation_token&);
	cancellation_token(cancellation_token&&);
	cancellation_token& operator=(const cancellation_token&);
	cancellation_token& operator=(cancellation_token&&);

public:
	cancellation_token()
		: state(false) {}

	bool is_canceled() const
	{
		bool s = state.load(std::memory_order_relaxed);
		if (s)
			std::atomic_thread_fence(std::memory_order_acquire);
		return s;
	}

	void cancel()
	{
		state.store(true, std::memory_order_release);
	}
};

// Cancel the current task by throwing a task_canceled exception. This is
// prefered to another exception type because it is handled more efficiently.
inline void cancel_current_task()
{
	LIBASYNC_THROW(task_canceled());
}

// Interruption point, calls cancel_current_task if the specified token is set.
inline void interruption_point(const cancellation_token& token)
{
	if (token.is_canceled())
		cancel_current_task();
}

} // namespace async
