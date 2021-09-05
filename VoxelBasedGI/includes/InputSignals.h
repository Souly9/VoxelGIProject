#pragma once
#include "boost/signals2/signal.hpp"
#include "boost/signals2/signal_type.hpp"
#include "boost/signals2/dummy_mutex.hpp"

namespace bs2 = boost::signals2;
using namespace bs2::keywords;

namespace SignalSystem
{
	class InputSignals
	{
		static std::unique_ptr<InputSignals> Events;
	public:
		// Remove real mutex to make Signals a bit more performant but more bulky to write
		bs2::signal_type<void(const glm::vec2&), mutex_type<bs2::dummy_mutex>>::type onMouseRightClick;
		bs2::signal_type<void(const glm::vec2&), mutex_type<bs2::dummy_mutex>>::type onMouseDrag;
		bs2::signal_type<void(const glm::vec2&), mutex_type<bs2::dummy_mutex>>::type onMouseRightReleased;
		bs2::signal_type<void(), mutex_type<bs2::dummy_mutex>>::type onMouseStartRightDrag;
		bs2::signal_type<void(const glm::vec2&), mutex_type<bs2::dummy_mutex>>::type onMouseStartLeftDrag;
		bs2::signal_type<void(), mutex_type<bs2::dummy_mutex>>::type onMouseStopRightDrag;
		bs2::signal_type<void(const glm::vec2&), mutex_type<bs2::dummy_mutex>>::type onMouseLeftDrag;
		bs2::signal_type<void(const glm::vec2&), mutex_type<bs2::dummy_mutex>>::type onMouseStopLeftDrag;
		bs2::signal_type<void(float), mutex_type<bs2::dummy_mutex>>::type onMouseScroll;
		bs2::signal_type<void(const glm::vec2&), mutex_type<bs2::dummy_mutex>>::type onMouseLeftClick;

		bs2::signal_type<void(const glm::vec4&), mutex_type<bs2::dummy_mutex>>::type onWASDPress;

		bs2::signal_type<void(), mutex_type<bs2::dummy_mutex>>::type onSpacePress;

		static InputSignals* Get()
		{
			if (Events)
				return Events.get();
			Events = std::make_unique<InputSignals>();
			return Events.get();
		}
	};
}
