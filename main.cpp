#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <sstream>
#include <iomanip>
using namespace std;

const int W = 1280;
const int H = 720;

enum State{
	SPLASHSCREEN, CLOSE
};

struct SplashScreenTex{
	SDL_Texture* normal;
	SDL_Rect rect;
};


class App{
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		TTF_Font* font;
		SDL_Event event;

		vector<SplashScreenTex> SST;
		State state;

		bool Init(){
			if(SDL_Init(SDL_INIT_VIDEO) < 0){
				cerr << "Gagal Inisialisasi SDL: " << SDL_GetError() << endl;
				return false;
			}
			if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
				cerr << "Gagal Inisialisasi SDL_image: " << IMG_GetError() << endl;
				return false;
			}
			if(TTF_Init() == -1){
				cerr << "Gagal Inisialisasi SDL_ttf: " << SDL_GetError() << endl;
				return false;
			}
			window = SDL_CreateWindow("Cat Defense", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, SDL_WINDOW_SHOWN);
			if(!window){
				cerr << "Gagal Membuat Window: " << SDL_GetError() << endl;
				return false;
			}
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			if(!renderer){
				cerr << "Gagal Membuat Renderer: " << SDL_GetError() << endl;
				return false;
			}
			font = TTF_OpenFont("res/fonts/NotoSans-Black.ttf", 48);
			if(!font){
				cerr << "Gagal Memuat Font: " << TTF_GetError() << endl;
				return false;
			}
			loadTextures();
			return true;
		}

		SDL_Color hexToRGB(const string& hexCode) {
			unsigned int hexValue;
			string hex = (hexCode[0] == '#') ? hexCode.substr(1) : hexCode;
			stringstream ss;
			ss << hex;
			ss >> hexValue;
		
			SDL_Color color;
			color.r = (hexValue >> 16) & 0xFF;
			color.g = (hexValue >> 8) & 0xFF;
			color.b = hexValue & 0xFF;
			color.a = 255;
		
			return color;
		}		

		SDL_Texture* imgToTexture(const char* path){
			SDL_Surface* surface = IMG_Load(path);
			if(!surface){
				cerr << "Gagal Memuat gambar dari " << path << endl;
				return nullptr;
			}
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);
			return texture;
		}

		SDL_Texture* ttfToTexture(const char* text, const string& hexCode){
			SDL_Surface* surface = TTF_RenderText_Solid(font, text, hexToRGB(hexCode));
			if(!surface){
				cerr << "Gagal Membuat Text: " << text << endl;
				return nullptr;
			}
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
			if(!texture){
				cerr << "Gagal Membuat Texture: " << SDL_GetError() << endl;
			}
			SDL_FreeSurface(surface);
			return texture;
		}

		void fillStructSST(const char* data, int mode, int x, int y, const string& hexCode = "#FFFFFF"){ // data bisa berupa path atau text, mode 0 untuk img, mode 1 untuk ttf
			SplashScreenTex tex;
			if(mode == 0){
				tex.normal = imgToTexture(data);
			}
			if(mode == 1){
				tex.normal = ttfToTexture(data, hexCode);
			}
			
			SDL_QueryTexture(tex.normal, nullptr, nullptr, &tex.rect.w, &tex.rect.h);
			tex.rect.x = x;
			tex.rect.y = y;
			SST.push_back(tex);
		}

		void loadTextures(){
			SDL_DisplayMode displayMode;
			fillStructSST("res/assets/splash_screen/splash_screen.png", 0, 0, 0);
			fillStructSST("Loading", 1, (W * 0.5), (H * 0.75));
			fillStructSST(" .", 1, (SST[1].rect.x + SST[1].rect.w), (H * 0.75) - (SST[1].rect.h / 2));
			fillStructSST(" .", 1, (SST[2].rect.x + SST[2].rect.w), (H * 0.75) - (SST[1].rect.h / 2));
			fillStructSST(" .", 1, (SST[3].rect.x + SST[3].rect.w), (H * 0.75) - (SST[1].rect.h / 2));
		}

		void SplashScreenUpdate(){
			int i = 0;
			for(const auto& s : SST){
				if(s.normal){
					SDL_RenderCopy(renderer, s.normal, nullptr, &s.rect); // bug
				}
			}
		}

		void eventHandle(){
			while(SDL_PollEvent(&event)){
				if(event.type == SDL_QUIT){
					state = CLOSE;
				}
			}
		}

		void SplashScreen(){
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);
			eventHandle();
			SplashScreenUpdate();
		}

		public:
			App() : window(nullptr), renderer(nullptr), font(nullptr), state(SPLASHSCREEN){}

			~App(){
				for(const auto& s : SST){
					SDL_DestroyTexture(s.normal);
				}
				if(font){
					TTF_CloseFont(font);
				}
				if(window){
					SDL_DestroyWindow(window);
				}
				if(renderer){
					SDL_DestroyRenderer(renderer);
				}
				TTF_Quit();
				IMG_Quit();
				SDL_Quit();
			}

			void run(){
				if(!Init()){
					cerr << "Gagal Inisialisasi Game!" << endl;
				}
				while(state != CLOSE){
					SplashScreen();
					SDL_RenderPresent(renderer);
					SDL_Delay(16);
				}
			}
};

int main(){
	App app;
	app.run();

	return 0;
}