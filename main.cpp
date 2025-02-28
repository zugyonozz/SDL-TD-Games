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
	SPLASHSCREEN, GAME, CLOSE
};

enum AnimState{
	IDLE, ATK
};

struct ImageTexData{
	SDL_Texture* normal;
	SDL_Rect rect;
};

struct Anim{
	SDL_Texture* normal;
	SDL_Rect dstRect;
	vector<SDL_Rect> srcRect;
};

class App{
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Event event;

		vector<ImageTexData> SST;
		vector<ImageTexData> G;
		vector<TTF_Font*> fonts;
		vector<Anim> anim;
		State state;
		AnimState animState;

		// Start Global Section
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
			initFonts();
			for(const auto& f : fonts){
				if(!f){
					cerr << "Gagal Memuat Font: " << TTF_GetError() << endl;
					return false;
				}
			}
			SSTLoadTextures();
			initAnim();
			return true;
		}

		void cleanUpRes(){
			for(const auto& s : SST){
				SDL_DestroyTexture(s.normal);
			}
			for(const auto& a : anim){
				SDL_DestroyTexture(a.normal);
			}
			for(const auto& f : fonts){
				TTF_CloseFont(f);
			}
		}

		void initFonts(){
			fonts.push_back(TTF_OpenFont("res/fonts/NotoSans-Regular.ttf", 72));
			fonts.push_back(TTF_OpenFont("res/fonts/NotoSans-SemiBold.ttf", 48));
			fonts.push_back(TTF_OpenFont("res/fonts/NotoSans-Black.ttf", 72));
			fonts.push_back(TTF_OpenFont("res/fonts/Planes-ValMore.ttf", 72));
		}

		SDL_Color hexToRGB(const string& hexCode) {
			unsigned int hexValue;
			string hex = (hexCode[0] == '#') ? hexCode.substr(1) : hexCode;
			
			stringstream ss;
			ss << std::hex << hex;
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

		SDL_Texture* ttfToTexture(const char* text, const string& hexCode, TTF_Font* font){
			SDL_Surface* surface = TTF_RenderText_Blended(font, text, hexToRGB(hexCode));
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
		// End GLobal Section

		// Start Splash Screen Section
		void imgToSST(const char* path, int x, int y){ // from img to sst 
			ImageTexData tex;
			tex.normal = imgToTexture(path);
			
			SDL_QueryTexture(tex.normal, nullptr, nullptr, &tex.rect.w, &tex.rect.h);
			tex.rect.x = x;
			tex.rect.y = y;
			SST.push_back(tex);
		}

		void ttfToSST(const char* text, SDL_Rect rect, int fontIndex, const string& hexCode = "FFFFFF"){ // jika rect.w dan rect.h di set 0 maka ukuran otomatif di sesuaikan dari hasil texture, fontIndex bisa di lihat di catatan paling bawah
			ImageTexData tex;
			tex.normal = ttfToTexture(text, hexCode, fonts[fontIndex]);

			if(rect.w && rect.h == 0){
				SDL_QueryTexture(tex.normal, nullptr, nullptr, &tex.rect.w, &tex.rect.h);
			}else{
				tex.rect.w = rect.w;
				tex.rect.h = rect.h;
			}
			tex.rect.x = rect.x;
			tex.rect.y = rect.y;
			SST.push_back(tex);
		}

		void SSTLoadTextures(){
			imgToSST("res/assets/splash_screen/splash-screen.webp", 0, 0);
			ttfToSST("Cat Defense", {static_cast<int>((W * 0.25)), static_cast<int>((H * 0.25)), static_cast<int>((W*0.50)), static_cast<int>((H * 0.25))}, 2);
			ttfToSST("Loading", {static_cast<int>((W * 0.42)), static_cast<int>((H * 0.75)), static_cast<int>((W * 0.12)), static_cast<int>((H * 0.06))}, 0);
		}

		void SplashScreenUpdate(){
			int i = 0;
			for(const auto& s : SST){
				if(s.normal){
					SDL_RenderCopy(renderer, s.normal, nullptr, &s.rect);
				}
			}
		}

		void SSEventHandle(){
			while(SDL_PollEvent(&event)){
				if(event.type == SDL_QUIT){
					state = CLOSE;
				}
			}
		}

		void SplashScreen(){
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);
			SSEventHandle();
			SplashScreenUpdate();
		}
		// End Splash Screen Section

		// Start Game Section
			void loadGameTextures(){
				// Check Point
			}

			void gameSpriteAnim(const char* path, int x, int y, int sizeFrame, int frameCount){
				Anim a;
				a.normal = imgToTexture(path);
				if (!a.normal) {
					cerr << "Gagal memuat gambar: " << path << " - " << IMG_GetError() << endl;
					return;
				}

				a.dstRect.x = x;
				a.dstRect.y = y;
				a.dstRect.w = sizeFrame;
				a.dstRect.h = sizeFrame;

				for(int i = 0; i < frameCount; i++){
					SDL_Rect frame;
					frame.x = i * sizeFrame;
					frame.y = 0;
					frame.w = sizeFrame;
					frame.h = sizeFrame;
					a.srcRect.push_back(frame);
				}
				anim.push_back(a);
			}

			void initAnim(){
				gameSpriteAnim("res/assets/game/Player/Pink_Monster_Idle.png", static_cast<int>((W*0.25)), static_cast<int>((H*0.25)), 32, 4);
			}

			void gameUpdate() {
				static Uint32 lastFrameTime = 0;
				int frameDelay = 100; // Delay per frame dalam ms
			
				for (auto& a : anim) {
					static int frameIndex = 0;
			
					Uint32 currentTime = SDL_GetTicks();
					if (currentTime > lastFrameTime + frameDelay) {
						frameIndex = (frameIndex + 1) % a.srcRect.size();
						lastFrameTime = currentTime;
					}
			
					SDL_RenderCopy(renderer, a.normal, &a.srcRect[frameIndex], &a.dstRect);
				}
			}

			void Game(){
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				SDL_RenderClear(renderer);
				gameUpdate();
			}
		
		public:
			App() : window(nullptr), renderer(nullptr), state(GAME){}

			~App(){
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
					while(SDL_PollEvent(&event)){
						if(event.type == SDL_QUIT){
							cleanUpRes();
							state = CLOSE;
						}
					}
					gameUpdate();
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

// fontIndex = {NotoSans-Regular, NotoSans-SemiBold, NotoSans-Black}