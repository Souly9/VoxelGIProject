#pragma once

namespace bs2 = boost::signals2;
using namespace bs2::keywords;

namespace SignalSystem
{
	class ApplicationEvents
	{
		static std::unique_ptr<ApplicationEvents> Events;

	public:
		// Remove real mutex to make Signals a bit more performant but more bulky to write
		bs2::signal_type<void(float), mutex_type<bs2::dummy_mutex>>::type onUpdate;
		bs2::signal_type<void(const glm::vec2&), mutex_type<bs2::dummy_mutex>>::type onScreenResize;

		bs2::signal_type<void(const glm::quat&), mutex_type<bs2::dummy_mutex>>::type onArcBallUpdate;
		bs2::signal_type<void(const glm::quat&), mutex_type<bs2::dummy_mutex>>::type onRotCamera;
		bs2::signal_type<void(), mutex_type<bs2::dummy_mutex>>::type onMainCameraValsChanged;

		static ApplicationEvents* Get()
		{
			if (Events)
				return Events.get();
			Events = std::make_unique<ApplicationEvents>();
			return Events.get();
		}
	};
}
