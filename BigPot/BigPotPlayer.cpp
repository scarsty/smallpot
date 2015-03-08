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
	cur_volume = BP_AUDIO_MIX_MAXVOLUME / 2;
	config->getInteger(cur_volume, "volume");
	UI->init();

	//首次运行拖拽的文件也认为是同一个
	drop_filename = filename;
	auto play_filename = filename;
	run = true;
	bool first = true;

	while (run)
	{
		media = nullptr;
		media = new BigPotMedia;

		//如果是控制台程序，通过参数传入的是ansi
		//如果是窗口程序，通过参数传入的是utf-8
		//所有通过拖拽传入的都是utf-8
		//播放器应以窗口程序为主
		play_filename = drop_filename;  //这两个都是utf8
		
		engine->setWindowTitle(play_filename);
		
		//打开文件, 需要进行转换
		auto open_filename = BigPotConv::conv(play_filename, BP_encode, sys_encode); //这个需要ansi
		media->openFile(open_filename);
		

		//窗口尺寸，时间
		media->videoStream->getSize(w, h);
		engine->setWindowSize(w, h);
		media->audioStream->setVolume(cur_volume);
		//首次打开文件窗口居中		
		if (first) engine->setWindowPosition(BP_WINDOWPOS_CENTERED, BP_WINDOWPOS_CENTERED);

		//读取记录中的文件时间并跳转
		cur_time = getFileTime(play_filename);
		media->seekTime(cur_time);

		//主循环
		engine->createMainTexture(w, h);
		this->eventLoop();
		engine->destroyMainTexture();

		//如果是媒体文件就记录时间
		if (media->isMedia())
			setFileTime(cur_time, play_filename);

		delete media;
		first = false;
	}
	config->setString(sys_encode, "sys_encode");
	config->setInteger(cur_volume, "volume");
	config->write();
	engine->destroy();
	return 0;
}

int BigPotPlayer::eventLoop()
{
	BP_Event e; 

	bool loop = true, pause = false;
	int ui_alpha = 128;
	int finished, i = 0, x, y;
	int t = 5000;
	int v = 4;
	bool havevideo = media->videoStream->exist();
	bool havemedia = media->audioStream->exist() || havevideo;
	int totalTime = media->getTotalTime();
	printf("Total time is %1.3fs or %dmin%ds\n", totalTime / 1000.0, totalTime / 60000, totalTime % 60000 / 1000);

	int maxDelay = 0;
	while (loop && engine->pollEvent(e) >= 0)
	{
		media->decodeFrame();
		engine->getMouseState(x, y);
		if (ui_alpha > 0) 
			ui_alpha--;
		if (h - y < 50 || (w - x) < 200 && y < 150)
			ui_alpha = 128;
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
			ui_alpha = 128;
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
			ui_alpha = 128;
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
			ui_alpha = 128;
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
			case BPK_RETURN:
				engine->toggleFullscreen();
				break;
			case BPK_ESCAPE:
				loop = false;
				run = false;
				break;
			}
			ui_alpha = 128;
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
				ui_alpha = 0;
			}
			break;
		case BP_DROPFILE:
			loop = false;
			drop_filename = e.drop.file;
			engine->free(e.drop.file);
		default:
			break;
		}
		e.type = BP_FIRSTEVENT;
		//media->audioStream->setAnotherTime(media->getVideoTime());
		//if (!media->showVideoFrame(i*100))
		int audioTime = media->getTime();  //注意优先为音频时间，若音频不存在使用视频时间
		if (havemedia && !pause)
		{
			int videostate = media->videoStream->showTexture(audioTime);
			//控制帧数
			if (videostate == 0)
			{
				UI->drawUI(ui_alpha, audioTime, totalTime, media->audioStream->changeVolume(0));
				engine->renderPresent();
				//以下均是为了显示信息，可以去掉
#ifdef _DEBUG
				int videoTime = (media->videoStream->getTimedts());
				int delay = -videoTime + audioTime;
				maxDelay = max(maxDelay, abs(delay));
				if (i % 1000 == 0)
				{
					maxDelay = 0;
				}
				printf("\rvolume %d, audio %4.3f, video %4.3f, diff %d / %d\t",
					media->audioStream->changeVolume(0), audioTime / 1e3, videoTime / 1e3, delay, i);
#endif
			}
			else if ((videostate == -1 || videostate == 2) && i % 50 == 0)
			{
				engine->renderCopy();
				UI->drawUI(ui_alpha, audioTime, totalTime, media->audioStream->changeVolume(0));
				engine->renderPresent();
			}
		}
		i++;
		engine->delay(1);
	}
	cur_time = media->getTime();
	cur_volume = media->audioStream->getVolume();
	engine->renderClear();
	engine->renderPresent();
	
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

std::string BigPotPlayer::getSysString(const string& str)
{
	return "";
}

int BigPotPlayer::getFileTime(const string& filename)
{
	if (filename == "")
		return 0;
	int time;
	config->getRecord(time, filename.c_str());
	return time;
}

int BigPotPlayer::setFileTime(int time, const string& filename)
{
	if (filename == "")
		return 0;
	config->setRecord(time, filename.c_str());
	return time;
}
