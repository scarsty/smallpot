#include "BigPotPlayer.h"


BigPotPlayer::BigPotPlayer()
{
	UI = new BigPotUI;
	config = new BigPotConfig;
}


BigPotPlayer::~BigPotPlayer()
{
	delete UI;
	delete config;
	//delete media;
}

int BigPotPlayer::beginWithFile(const string &filename)
{
	if (engine->init()) return -1;
	
	config->init();
	config->getString(sys_encode, "sys_encode");
	volume = BP_AUDIO_MIX_MAXVOLUME / 2;
	config->getInteger(volume, "volume");
	UI->init();

	this->filename = filename;
	run = true;
	bool first = true;
	while (run)
	{
		media = nullptr;
		media = new BigPotMedia;

		media->openFile(this->filename);

		engine->getWindowSize(w, h);
		media->videoStream->getSize(w, h);
		media->audioStream->setVolume(volume);

		engine->setWindowSize(w, h);
		engine->createMainTexture(w, h);
		if (first) engine->setWindowPosition(BP_WINDOWPOS_CENTERED, BP_WINDOWPOS_CENTERED);
		auto s = BigPotConv::conv(this->filename, sys_encode, BP_encode);
		engine->setWindowTitle(s);

		this->eventLoop();

		volume = media->audioStream->getVolume();
		engine->destroyMainTexture();
		engine->renderClear();
		engine->renderPresent();
		delete media;
		first = false;
	}
	config->setString(sys_encode, "sys_encode");
	config->setInteger(volume, "volume");
	config->write();
	engine->destroy();
	return 0;
}

int BigPotPlayer::eventLoop()
{
	BP_Event e; 

	bool loop = true, pause = false;
	int drawUI = 128;
	int finished, i=0, x, y;
	int t = 5000;
	int v = 4;
	bool havemedia = media->audioStream->exist() || media->videoStream->exist();
	int totalTime = media->getTotalTime();
	printf("Total time is %1.3fs or %dmin%ds\n", totalTime / 1000.0, totalTime / 60000, totalTime % 60000 / 1000);

	int maxDelay = 0;
	while (loop && engine->pollEvent(e) >= 0)
	{
		media->decodeFrame();
		engine->getMouseState(x, y);
		if (drawUI > 0) 
			drawUI--;
		if (h - y < 50 || (w - x) < 200 && y < 150)
			drawUI = 128;
		switch (e.type)
		{
		case BP_MOUSEMOTION:
			break;
		case BP_MOUSEBUTTONUP:
			if (e.button.button == BP_BUTTON_LEFT)
			{
				if (h - e.button.y < 50)
				{
					double pos = 1.0 * e.button.x / w;
					media->seekPos(pos);
				}

				if (e.button.y < 50 && e.button.x > w - 100)
				{
					pause = !pause;
					media->audioStream->setPause(pause);
				}
			}
			drawUI = 128;
			break;
		case BP_MOUSEWHEEL:
		{
			if (e.wheel.y > 0)
			{
				media->audioStream->changeVolume(v);
			}
			else if (e.wheel.y < 0)
			{
				media->audioStream->changeVolume(-v);
			}
			drawUI = 128;
			break;
		}
		case BP_KEYDOWN:
		{
			switch (e.key.keysym.sym)
			{
			case BPK_LEFT:
				media->seekTime(media->getTime() - t, -1);
				break;
			case BPK_RIGHT:
				media->seekTime(media->getTime() + t);
				break;
			case BPK_UP:
				media->audioStream->changeVolume(v);
				break;
			case BPK_DOWN:
				media->audioStream->changeVolume(-v);
				break;
			}
			drawUI = 128;
			break;
		}
		case BP_KEYUP:
		{
			switch (e.key.keysym.sym)
			{
			case BPK_SPACE:
				pause = !pause;
				media->audioStream->setPause(pause);
				break;
			}
			drawUI = 128;
			break;
		}
		case BP_QUIT:
			loop = false;
			run = false;
			break;
		case BP_WINDOWEVENT:
			if (e.window.event == BP_WINDOWEVENT_RESIZED)
			{
				w = e.window.data1;
				h = e.window.data2;
			}
			else if (e.window.event == BP_WINDOWEVENT_LEAVE)
			{
				drawUI = 0;
			}
			break;
		case BP_DROPFILE:
			loop = false;
			filename = BigPotConv::conv(e.drop.file, BP_encode, sys_encode);
			engine->free(e.drop.file);
		default:
			break;
		}
		e.type = BP_FIRSTEVENT;
		//media->audioStream->setAnotherTime(media->getVideoTime());
		//if (!media->showVideoFrame(i*100))
		int audioTime = media->getTime();  //注意优先为音频时间，若音频不存在使用视频时间增加2微秒
		if (havemedia && !pause 
			&& !media->videoStream->showTexture(audioTime))
		{
			UI->drawUI(drawUI, audioTime, totalTime, media->audioStream->changeVolume(0));
			engine->renderPresent();

			//以下均是为了显示信息，可以全部去掉
			int videoTime = (media->videoStream->getTimedts());
			int delay = -videoTime + audioTime;
			maxDelay = max(maxDelay, abs(delay));
			if (i % 1000 == 0)
			{
				maxDelay = 0;
			}
			printf("\rvolume %d, audio %4.3f, video %4.3f, diff %d / %d\t",
				media->audioStream->changeVolume(0), audioTime / 1e3, videoTime / 1e3, delay, i);
		}
		i++;
		engine->delay(1);
	}
	return 0;
}

int BigPotPlayer::drawTex2()
{
	/*SDL_SetRenderTarget(ren, tex2);

	SDL_Texture * img = IMG_LoadTexture(ren, "logo.png");	

	SDL_Rect r;
	SDL_RenderCopy(ren, img, nullptr, nullptr);
	SDL_DestroyTexture(img);

	SDL_SetRenderTarget(ren, nullptr);
	*/
	return 0;
}

int BigPotPlayer::showTex2()
{
	//SDL_RenderCopy(ren, tex2, nullptr, nullptr);
	return 0;
}
