// audio.hpp

#pragma once

namespace realware
{
	class iSoundAPI;

	class cAudio : public iObject
	{
		REALWARE_CLASS(cAudio)

	public:
		enum class API
		{
			NONE = 0,
			OAL,
		};

	public:
		explicit cAudio(cContext* context);
		virtual ~cAudio();

		void SetAPI(API api);

	private:
		iSoundAPI* _context = nullptr;
	};
}