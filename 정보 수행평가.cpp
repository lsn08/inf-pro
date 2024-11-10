#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <chrono>
using namespace std;

#define INVENTORY_FILE_PREFIX "inventory_item_" 
#define SLEEP(milliseconds) Sleep(milliseconds)
#define MAX_INVENTORY_SIZE 100
#define MAX_INVENTORY_WEIGHT 100
#define SAVE_FILE "save_data.txt"
#define CONSOLE_WIDTH 170
#define CONSOLE_HEIGHT 100
int gold = 0;
int fuell = 100;
bool has_fishing_master = false;
bool has_fishing_grandmaster = false;
bool has_fishing_god = false;
bool has_int_title = false;

std::random_device rd;
std::default_random_engine engine(rd());

void set_console_font_size(int size);
void center_text(const char* text);
void show_controls();
void show_intro();
void save_game_data();
void load_game_data();
void buy_name();
void upgrade_ship();
void buy_fuel();
void enter_shop();
void set_console_color(int color);
void initialize_layer(int layer);
void initialize_map();
void initialize_map2();
void display_current_terrain();
void print_map();
void start_fishing();
void refuel();
void move_towards_target();
void reset_layer(int previous_layer);
void change_layer(char direction);
void move_ship(char direction);



struct Node {
    int x, y;
    int parent_x, parent_y;
};

void set_console_font_size(int size) {
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = size;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy(cfi.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}

void center_text(const char* text) {
    int len = strlen(text);
    int padding = (CONSOLE_WIDTH - len) / 2;

    for (int i = 0; i < padding; i++) {
        printf(" ");
    }
    printf("%s\n", text);
}


void show_controls() {
    system("cls");
    center_text("===== 조작법 =====");
    center_text("w: 위로 이동");
    center_text("s: 아래로 이동");
    center_text("a: 왼쪽으로 이동");
    center_text("d: 오른쪽으로 이동");
    center_text("f: 어획 시도");
    center_text("k: 연료 충전");
    center_text("t: 목표 좌표 설정");
    center_text("q: 게임 종료");
    center_text("b: 상점 진입");
    printf("\n");
    center_text("        아무 키나 눌러서 메인 화면으로 돌아가세요.");
    _getch();
    system("cls");
}

void show_intro() {
    while (1) {
        int temp = 0;
        while (temp == 0) {
            system("cls");
            center_text("원양어선 시뮬레이터에 오신 것을 환영합니다!");
            SLEEP(2000);
            temp++;
        }

        system("cls");


        center_text("\033[1;33m██\033[0m: 어선 (노란색)");           // 노란색
        center_text("\033[1;32m██\033[0m: 목표 지점 (초록색)");       // 초록색
        center_text("\033[0;32m██\033[0m: 어획 가능 지점 (진한 초록색)"); // 진한 초록색
        center_text("\033[1;35m██\033[0m: 연료 섬 (분홍색)");         // 분홍색
        center_text("\033[1;31m██\033[0m: 장애물 (빨간색)");          // 빨간색
        center_text("\033[1;36m██\033[0m: 바다 (청록색)");           // 청록색
        center_text("\033[0;36m██\033[0m: 암석 지대 (암석 청록색)");    // 암석 청록색
        center_text("\033[0;34m██\033[0m: 얕은 물 (진한 파란색)");     // 진한 파란색
        center_text("\033[1;34m██\033[0m: 차가운 물 (파란색)");       // 파란색
        center_text("\033[1;37m██\033[0m: 천국 물 (흰색)");           // 흰색

        printf("\n\n");

        center_text("엔터를 눌러 게임을 시작하세요.");
        center_text("'?'을 눌러 조작법을 알아보세요.");

        char command = _getch();

        if (command == '?') {
            show_controls();
        }
        else if (command == 13) {
            break;
        }
    }
}




#define MAP_WIDTH 50
#define MAP_HEIGHT 25
#define NUM_LAYERS 9
#define NUM_TERRAINS 5

typedef struct {
    int giving_money;
    char name[40];
    int fish_status;
    int fish_size;
} FishCell;

// 전체 물고기 목록
FishCell fish_types[] = {
    // Ocean 지형
    {100, "참다랑어", 1, 5},
    {120, "눈다랑어", 2, 6},
    {90, "바라쿠다", 1, 4},
    {110, "방어", 2, 5},
    {150, "돛새치", 2, 7},
    {300, "고래상어", 1, 10},
    {130, "잿방어", 2, 6},
    {400, "백상아리", 1, 9},
    {140, "다금바리", 1, 6},
    {130, "만새기", 2, 5},

    // Rocky 지형
    {90, "참돔", 1, 4},
    {70, "돌돔", 0, 3},
    {120, "자바리", 2, 6},
    {100, "부시리", 1, 5},
    {80, "곰치", 1, 4},
    {200, "바닷가재", 2, 4},
    {100, "전복", 1, 3},
    {250, "대왕조개", 1, 8},
    {130, "호랑이 자바리", 2, 6},
    {80, "고둥", 1, 3},

    // Shallow 지형
    {60, "가자미", 1, 3},
    {80, "노랑가오리", 1, 5},
    {50, "모래무지", 1, 2},
    {70, "감성돔", 1, 3},
    {40, "백조기", 0, 2},
    {90, "황돔", 1, 3},
    {100, "망둥어", 2, 4},
    {80, "꽃게", 1, 3},
    {60, "강도다리", 1, 3},
    {90, "참치", 1, 4},

    // Cold 지형
    {100, "대구", 1, 4},
    {70, "명태", 1, 3},
    {120, "연어", 2, 5},
    {110, "북극곤들매기", 1, 4},
    {140, "할리벗", 2, 6},
    {80, "청어", 0, 3},
    {300, "그린란드상어", 1, 8},
    {130, "코호 연어", 2, 4},
    {140, "홍연어", 1, 5},
    {200, "대게", 2, 3},

    // Heaven 지형
    {1000, "리바이어던 파이크", 2, 10},
    {800, "천상의 청새치", 2, 9},
    {750, "심연 가오리", 1, 8},
    {900, "별빛 돔", 2, 8},
    {1100, "크라켄 발톱", 1, 9},
    {950, "유령 참치", 2, 7},
    {1000, "용지느러미", 2, 8},
    {850, "달빛 자바리", 1, 7},
    {1050, "세라핌 새우", 2, 6},
    {900, "물불뱀장어", 1, 7}
};

// Ocean 지형 물고기 목록
FishCell ocean_fish[] = {
    {100, "참다랑어", 1, 5},
    {120, "눈다랑어", 2, 6},
    {90, "바라쿠다", 1, 4},
    {110, "방어", 2, 5},
    {150, "돛새치", 2, 7},
    {300, "고래상어", 1, 10},
    {130, "잿방어", 2, 6},
    {400, "백상아리", 1, 9},
    {140, "다금바리", 1, 6},
    {130, "만새기", 2, 5}
};

// Rocky 지형 물고기 목록
FishCell rocky_fish[] = {
    {90, "참돔", 1, 4},
    {70, "돌돔", 0, 3},
    {120, "자바리", 2, 6},
    {100, "부시리", 1, 5},
    {80, "곰치", 1, 4},
    {200, "바닷가재", 2, 4},
    {100, "전복", 1, 3},
    {250, "대왕조개", 1, 8},
    {130, "호랑이 자바리", 2, 6},
    {80, "고둥", 1, 3}
};

// Shallow 지형 물고기 목록
FishCell shallow_fish[] = {
    {60, "가자미", 1, 3},
    {80, "노랑가오리", 1, 5},
    {50, "모래무지", 1, 2},
    {70, "감성돔", 1, 3},
    {40, "백조기", 0, 2},
    {90, "황돔", 1, 3},
    {100, "망둥어", 2, 4},
    {80, "꽃게", 1, 3},
    {60, "강도다리", 1, 3},
    {90, "참치", 1, 4}
};

// Cold 지형 물고기 목록
FishCell cold_fish[] = {
    {100, "대구", 1, 4},
    {70, "명태", 1, 3},
    {120, "연어", 2, 5},
    {110, "북극곤들매기", 1, 4},
    {140, "할리벗", 2, 6},
    {80, "청어", 0, 3},
    {300, "그린란드상어", 1, 8},
    {130, "코호 연어", 2, 4},
    {140, "홍연어", 1, 5},
    {200, "대게", 2, 3}
};

// Heaven 지형 물고기 목록
FishCell Heaven_fish[] = {
    {1000, "리바이어던 파이크", 2, 10},
    {800, "천상의 청새치", 2, 9},
    {750, "심연 가오리", 1, 8},
    {900, "별빛 돔", 2, 8},
    {1100, "크라켄 발톱", 1, 9},
    {950, "유령 참치", 2, 7},
    {1000, "용지느러미", 2, 8},
    {850, "달빛 자바리", 1, 7},
    {1050, "세라핌 새우", 2, 6},
    {900, "물불뱀장어", 1, 7}
};

struct InventoryItem {
    FishCell fish;
    int quantity;
    int total_weight;
};

InventoryItem inventory[MAX_INVENTORY_SIZE];
int inventory_count = 0;
int current_weight = 0;
int max_inventory_weight = MAX_INVENTORY_WEIGHT;
typedef struct {
    int velocity;
    int fuel2;
} Shipcell;

typedef struct {
    char terrain;
    char object;
} MapCell;

MapCell map[NUM_LAYERS][MAP_HEIGHT][MAP_WIDTH];
MapCell map2[MAP_HEIGHT][MAP_WIDTH];
int ship_x = 0;
int ship_y = 0;
int current_layer = 0;
int target_x = -1;
int target_y = -1;
int obstacles = 0;
int total_fish_caught = 0;
HANDLE consoleHandle;
Shipcell myship = { 1,100 };

void upgrade_ship() {
    int upgrade_choice;
    printf("업그레이드 옵션을 선택하세요:\n");
    printf("1. 속도 증가 (비용: 700)\n");
    printf("2. 연료 용량 증가 (비용: 500)\n");
    printf("3. 인벤토리 용량 증가 (비용: 300)\n");
    printf("선택을 입력하세요: ");
    if (scanf("%d", &upgrade_choice) != 1 || upgrade_choice < 1 || upgrade_choice > 3) {
        printf("잘못된 선택입니다! 다시 시도하세요.\n");
        return;
    }

    if (upgrade_choice == 1 && gold >= 700) {
        myship.velocity += 1;
        gold -= 700;
        printf("속도가 업그레이드되었습니다! 현재 속도: %d\n", myship.velocity);
    }
    else if (upgrade_choice == 2 && gold >= 500) {
        myship.fuel2 += 50;
        gold -= 500;
        printf("연료 용량이 업그레이드되었습니다! 현재 연료 용량: %d\n", myship.fuel2);
    }
    else if (upgrade_choice == 3 && gold >= 300) {
        max_inventory_weight += 100;
        gold -= 300;
        printf("인벤토리 용량이 업그레이드되었습니다! 현재 인벤토리 용량: %d\n", max_inventory_weight);
    }
    else {
        printf("골드가 부족합니다!\n");
    }
}



void buy_name() {
    int name_choice;
    printf("\n구매할 칭호를 선택하세요:\n");
    printf("1. '낚시 고수' (100,000 골드) - 속도 +1\n");
    printf("2. '낚시 초고수' (500,000 골드) - 연료 용량 +50\n");
    printf("3. '낚시의 신' (5,000,000 골드) - 속도 +2, 연료 용량 +100\n");
    printf("4. 'int' (2,147,480,000 골드) - 속도 +100, 연료 용량 +500000\n");
    printf("Enter your choice (1-4): ");
    if (scanf("%d", &name_choice) != 1 || name_choice < 1 || name_choice > 4) {
        printf("잘못된 선택입니다! 다시 시도하세요.\n");
        return;
    }

    if (name_choice == 1 && gold >= 100000) {
        myship.velocity += 1;
        gold -= 100000;
        has_fishing_master = true;
        printf("\n칭호 '낚시 고수'를 구매했습니다! 현재 속도: %d\n", myship.velocity);
    }
    else if (name_choice == 2 && gold >= 500000) {
        myship.fuel2 += 50;
        gold -= 500000;
        has_fishing_grandmaster = true;
        printf("\n칭호 '낚시 초고수'를 구매했습니다! 현재 연료 용량: %d\n", myship.fuel2);
    }
    else if (name_choice == 3 && gold >= 5000000) {
        myship.velocity += 2;
        myship.fuel2 += 100;
        gold -= 5000000;
        has_fishing_god = true;
        printf("\n칭호 '낚시의 신'을 구매했습니다! 현재 속도: %d, 현재 연료 용량: %d\n", myship.velocity, myship.fuel2);
    }
    else if (name_choice == 4 && gold >= 2147480000) {
        myship.velocity += 100;
        myship.fuel2 += 500000;
        gold -= 2147480000;
        has_int_title = true;
        printf("\n칭호 'int'를 구매했습니다! 현재 속도: %d, 현재 연료 용량: %d\n", myship.velocity, myship.fuel2);
    }
    else {
        printf("\n골드가 부족합니다. 현재 골드: %d\n", gold);
    }
}


void sell_fish() {
    if (inventory_count == 0) {
        printf("인벤토리가 비어 있습니다! 판매할 물고기가 없습니다.\n");
        return;
    }

    printf("\n현재 인벤토리:\n");
    for (int i = 0; i < inventory_count; i++) {
        printf("%d. %s - 수량: %d, 총 무게: %d, 판매 가격: %d\n",
            i + 1, inventory[i].fish.name, inventory[i].quantity,
            inventory[i].total_weight, static_cast<int>(inventory[i].fish.giving_money * inventory[i].quantity * 1.5));
    }

    int choice;
    printf("판매할 물고기의 번호를 입력하세요 (종료하려면 0): ");
    if (scanf("%d", &choice) != 1 || choice < 0 || choice > inventory_count) {
        printf("잘못된 선택입니다! 다시 시도하세요.\n");
        return;
    }

    if (choice > 0 && choice <= inventory_count) {
        int index = choice - 1;
        int sale_value = static_cast<int>(inventory[index].fish.giving_money * inventory[index].quantity * 1.5);
        gold += sale_value;
        int new_weight = current_weight - inventory[index].total_weight;
        current_weight = (new_weight >= 0) ? new_weight : 0;

        printf("%d개의 %s(을)를 %d 골드에 판매했습니다! 현재 골드: %d\n",
            inventory[index].quantity, inventory[index].fish.name, sale_value, gold);

        for (int i = index; i < inventory_count - 1; i++) {
            inventory[i] = inventory[i + 1];
        }
        inventory_count--;
    }
}


void enter_shop() {
    char shop_command;
    while (1) {
        printf("\n선박 상점에 오신 것을 환영합니다!\n");
        printf("1. 칭호 구매\n");
        printf("2. 배 업그레이드\n");
        printf("3. 물고기 판매\n");
        printf("q. 상점 나가기\n");
        printf("선택을 입력하세요: ");
        shop_command = _getch();

        if (shop_command == '1') {
            buy_name();
        }
        else if (shop_command == '2') {
            upgrade_ship();
        }
        else if (shop_command == '3') {
            sell_fish();
        }
        else if (shop_command == 'q') {
            break;
        }
        else {
            printf("잘못된 선택입니다! 다시 시도하세요.\n");
        }
    }
}

// 개별 데이터 파일 저장 함수
void save_inventory_capacity() {
    FILE* file = fopen("inventory_capacity.txt", "w");
    if (file) {
        fprintf(file, "%d", max_inventory_weight);
        fclose(file);
    }
}

void save_gold() {
    FILE* file = fopen("gold.txt", "w");
    if (file) {
        fprintf(file, "%d", gold);
        fclose(file);
    }
}

void save_velocity() {
    FILE* file = fopen("velocity.txt", "w");
    if (file) {
        fprintf(file, "%d", myship.velocity);
        fclose(file);
    }
}

void save_fuel_capacity() {
    FILE* file = fopen("fuel_capacity.txt", "w");
    if (file) {
        fprintf(file, "%d", myship.fuel2);
        fclose(file);
    }
}



void save_titles() {
    FILE* file = fopen("titles.txt", "w");
    if (file) {
        fprintf(file, "Fishing Master: %d\n", has_fishing_master);
        fprintf(file, "Fishing Grandmaster: %d\n", has_fishing_grandmaster);
        fprintf(file, "Fishing God: %d\n", has_fishing_god);
        fprintf(file, "Int Title: %d\n", has_int_title);
        fclose(file);
    }
}

void save_inventory() {
    for (int i = 0; i < inventory_count; i++) {
        char filename[50];
        snprintf(filename, sizeof(filename), "%s%d.txt", INVENTORY_FILE_PREFIX, i);
        FILE* file = fopen(filename, "w");
        if (file) {
            fprintf(file, "Name: %s\n", inventory[i].fish.name);
            fprintf(file, "Giving Money: %d\n", inventory[i].fish.giving_money);
            fprintf(file, "Fish Status: %d\n", inventory[i].fish.fish_status);
            fprintf(file, "Fish Size: %d\n", inventory[i].fish.fish_size);
            fprintf(file, "Quantity: %d\n", inventory[i].quantity);
            fprintf(file, "Total Weight: %d\n", inventory[i].total_weight);
            fclose(file);
        }
    }
}

// 게임 데이터 저장 함수
void save_game_data() {
    save_gold();
    save_velocity();
    save_fuel_capacity();
    save_titles();
    save_inventory();
    save_inventory_capacity();
}

// 개별 데이터 파일 불러오기 함수
void load_inventory_capacity() {
    FILE* file = fopen("inventory_capacity.txt", "r");
    if (file) {
        fscanf(file, "%d", &max_inventory_weight);
        fclose(file);
    }
    else {
        max_inventory_weight = MAX_INVENTORY_WEIGHT;
    }
}

void load_gold() {
    FILE* file = fopen("gold.txt", "r");
    if (file) {
        fscanf(file, "%d", &gold);
        fclose(file);
    }
}

void load_velocity() {
    FILE* file = fopen("velocity.txt", "r");
    if (file) {
        fscanf(file, "%d", &myship.velocity);
        fclose(file);
    }
}

void load_fuel_capacity() {
    FILE* file = fopen("fuel_capacity.txt", "r");
    if (file) {
        fscanf(file, "%d", &myship.fuel2);
        fclose(file);
    }
}


void load_titles() {
    FILE* file = fopen("titles.txt", "r");
    if (file) {
        int temp;
        fscanf(file, "Fishing Master: %d\n", &temp);
        has_fishing_master = (temp == 1);

        fscanf(file, "Fishing Grandmaster: %d\n", &temp);
        has_fishing_grandmaster = (temp == 1);

        fscanf(file, "Fishing God: %d\n", &temp);
        has_fishing_god = (temp == 1);

        fscanf(file, "Int Title: %d\n", &temp);
        has_int_title = (temp == 1);

        fclose(file);
    }
}

void load_inventory() {
    inventory_count = 0;
    while (1) {
        char filename[50];
        snprintf(filename, sizeof(filename), "%s%d.txt", INVENTORY_FILE_PREFIX, inventory_count);
        FILE* file = fopen(filename, "r");
        if (file == NULL) {
            break;
        }

        fscanf(file, "Name: %39s\n", inventory[inventory_count].fish.name);
        fscanf(file, "Giving Money: %d\n", &inventory[inventory_count].fish.giving_money);
        fscanf(file, "Fish Status: %d\n", &inventory[inventory_count].fish.fish_status);
        fscanf(file, "Fish Size: %d\n", &inventory[inventory_count].fish.fish_size);
        fscanf(file, "Quantity: %d\n", &inventory[inventory_count].quantity);
        fscanf(file, "Total Weight: %d\n", &inventory[inventory_count].total_weight);

        fclose(file);
        inventory_count++;
    }
}

// 게임 데이터 불러오기 함수
void load_game_data() {
    load_gold();
    load_velocity();
    load_fuel_capacity();
    load_titles();
    load_inventory();
    load_inventory_capacity();
}


// 콘솔 텍스트 색상 설정 함수
void set_console_color(int color) {
    SetConsoleTextAttribute(consoleHandle, color);
}


void initialize_layer(int layer) {
    // 완전 난수를 위한 시간 기반 시드
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 random_engine(seed);
    std::uniform_int_distribution<int> terrain_dist(0, 999);

    int terrain_type = terrain_dist(random_engine);  // 0 ~ 999 사이의 난수 생성
    char terrain_char;

    // 지형 타입에 따라 문자를 설정
    if (terrain_type < 400) {
        terrain_char = '~';  // Ocean (40% 확률)
    }
    else if (terrain_type < 575) {
        terrain_char = 'R';  // Rocky (17.5% 확률)
    }
    else if (terrain_type < 775) {
        terrain_char = 'S';  // Shallow water (20% 확률)
    }
    else if (terrain_type < 999) {
        terrain_char = 'C';  // Cold water (17.4% 확률)
    }
    else {
        terrain_char = 'H';  // Fish's Heaven (0.1% 확률)
    }

    // 모든 셀에 동일한 지형 타입 적용
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            map[layer][i][j].terrain = terrain_char;
            map[layer][i][j].object = '\0';
        }
    }

    // 어획 가능 지점과 장애물을 레이어에 배치
    std::uniform_int_distribution<int> height_dist(0, MAP_HEIGHT - 1);
    std::uniform_int_distribution<int> width_dist(0, MAP_WIDTH - 1);

    // 어획 가능 지점 배치
    int fish_zones = (terrain_char == 'H') ? 50 : 20;  // Fish's Heaven일 때 어획 가능 지점 증가
    for (int i = 0; i < fish_zones; i++) {
        int x = height_dist(random_engine);
        int y = width_dist(random_engine);
        map[layer][x][y].object = '*';
    }

    // 장애물 배치
    int obstacle_count = (terrain_char == 'R') ? 20 : 10;  // Rocky 지형일 때 장애물 증가
    for (int i = 0; i < obstacle_count; i++) {
        int x = height_dist(random_engine);
        int y = width_dist(random_engine);
        if (map[layer][x][y].object == '\0') {
            map[layer][x][y].object = 'X';
        }
    }

    // 연료 보급 가능한 섬 배치
    int fuel_islands = 3;
    for (int i = 0; i < fuel_islands; i++) {
        int x = height_dist(random_engine) % (MAP_HEIGHT - 2);
        int y = width_dist(random_engine) % (MAP_WIDTH - 2);
        for (int dx = 0; dx < 3; dx++) {
            for (int dy = 0; dy < 3; dy++) {
                map[layer][x + dx][y + dy].object = 'I';
            }
        }
    }
}


// 전체 맵 초기화 함수
void initialize_map() {
    srand((unsigned int)time(NULL));  // 랜덤 시드 설정 (프로그램 시작 시 한 번만 호출)
    for (int layer = 0; layer < NUM_LAYERS; layer++) {
        initialize_layer(layer);  // 각 레이어 초기화
    }

    // 어선의 초기 위치 표시
    map[current_layer][ship_x][ship_y].object = 'B';
}

// 현재 레이어의 정보를 map2에 복사하는 함수
void initialize_map2() {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            map2[i][j] = map[current_layer][i][j];
        }
    }
}

// 맵을 출력하는 함수
void display_current_terrain() {
    char terrain = map2[ship_x][ship_y].terrain;
    printf("현재 지형: ");
    printf("%c\n", terrain);
}

void print_map() {
    initialize_map2();  // 현재 레이어의 정보를 map2로 복사
    system("cls");  // 화면을 지웁니다 (Windows). Linux/Unix에서는 "clear"를 사용할 수 있습니다.
    display_current_terrain();

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            // 목표 지점 표시
            if (i == target_x && j == target_y) {
                set_console_color(10);  // 초록색 (목표 지점)
                printf("██");
            }
            // 레이어에 어선이 있으면 출력, 없으면 객체 맵의 정보를 출력
            else if (map2[i][j].object == 'B') {
                set_console_color(14);  // 노란색 (어선)
                printf("██");
            }
            else if (map2[i][j].object == '*') {
                set_console_color(2);  // 초록색 (어획 가능 지점)
                printf("██");
            }
            else if (map2[i][j].object == 'I') {
                set_console_color(13);  // 파란색 (연료 섬)
                printf("██");
            }
            else if (map2[i][j].object == 'X') {
                set_console_color(4);  // 빨간색 (장애물)
                printf("██");
            }
            else {
                // 기본 색상 설정 (지형에 따라 다르게 설정)
                switch (map2[i][j].terrain) {
                case '~':  // 바다
                    set_console_color(11);  // 청록색
                    break;
                case 'R':  // 암석 지대
                    set_console_color(3);  // 청록색
                    break;
                case 'S':  // 얕은 물
                    set_console_color(1);  // 진한 파란색
                    break;
                case 'C':  // 차가운 물
                    set_console_color(9);  // 파란색
                    break;
                default:
                    set_console_color(7);  // 기본 색상 (흰색)
                    break;
                }
                printf("██");
            }
        }
        printf("\n");
    }
    printf("연료: %d\n", fuell);  // 연료량 출력
    printf("총 어획량: %d\n", total_fish_caught);  // 총 어획량 출력
    set_console_color(7);  // 색상 복원
}

void start_fishing() {
    std::random_device rd;
    std::mt19937 gen(rd());
    FishCell* fish_pool;
    int fish_count;

    char current_terrain = map[current_layer][ship_x][ship_y].terrain;
    switch (current_terrain) {
    case '~': fish_pool = ocean_fish; fish_count = sizeof(ocean_fish) / sizeof(ocean_fish[0]); break;
    case 'R': fish_pool = rocky_fish; fish_count = sizeof(rocky_fish) / sizeof(rocky_fish[0]); break;
    case 'S': fish_pool = shallow_fish; fish_count = sizeof(shallow_fish) / sizeof(shallow_fish[0]); break;
    case 'C': fish_pool = cold_fish; fish_count = sizeof(cold_fish) / sizeof(cold_fish[0]); break;
    case 'H': fish_pool = Heaven_fish; fish_count = sizeof(Heaven_fish) / sizeof(Heaven_fish[0]); break;
    default: printf("알 수 없는 지형입니다. 물고기를 찾을 수 없습니다.\n"); return;
        SLEEP(1000);
    }

    int total_weight = 0;
    for (int i = 0; i < fish_count; i++) {
        total_weight += 1000 / fish_pool[i].giving_money;
    }

    std::uniform_int_distribution<> weight_dist(0, total_weight - 1);
    int random_weight = weight_dist(gen);
    int cumulative_weight = 0;
    FishCell* caught_fish = nullptr;

    for (int i = 0; i < fish_count; i++) {
        cumulative_weight += 1000 / fish_pool[i].giving_money;
        if (random_weight < cumulative_weight) {
            caught_fish = &fish_pool[i];
            break;
        }
    }

    if (caught_fish == nullptr) {
        printf("어획에 실패했습니다. 오류가 발생했습니다.\n");
        SLEEP(1000);
        return;
    }

    std::uniform_int_distribution<> size_dist(-3, 3);
    int size_adjustment = size_dist(gen);
    int original_size = caught_fish->fish_size;
    caught_fish->fish_size = original_size + size_adjustment;
    if (caught_fish->fish_size < 1) {
        caught_fish->fish_size = 1;
    }

    int fish_weight = caught_fish->fish_size * 10;  // 물고기 크기당 10 무게로 가정
    if (current_weight + fish_weight > max_inventory_weight) {
        printf("인벤토리가 가득 찼습니다! %s을(를) 추가할 수 없습니다. 현재 무게: %d/%d\n", caught_fish->name, current_weight, max_inventory_weight);
        SLEEP(1000);
        return;
    }

    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            int nx = ship_x + dx;
            int ny = ship_y + dy;

            if (nx >= 0 && nx < MAP_HEIGHT && ny >= 0 && ny < MAP_WIDTH) {
                if (map[current_layer][nx][ny].object == '*') {
                    printf("(%d, %d) 근처에서 어획 성공! %s을(를) 잡았습니다!\n", nx, ny, caught_fish->name);
                    SLEEP(1000);
                    map[current_layer][nx][ny].object = '\0';
                    total_fish_caught++;

                    // 인벤토리에 물고기를 추가
                    bool found = false;
                    for (int i = 0; i < inventory_count; i++) {
                        if (strcmp(inventory[i].fish.name, caught_fish->name) == 0) {
                            inventory[i].quantity++;
                            inventory[i].total_weight += fish_weight;
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        inventory[inventory_count].fish = *caught_fish;
                        inventory[inventory_count].quantity = 1;
                        inventory[inventory_count].total_weight = fish_weight;
                        inventory_count++;
                    }

                    // 현재 인벤토리 무게 갱신
                    current_weight += fish_weight;
                    printf("물고기가 인벤토리에 추가되었습니다! 현재 무게: %d/%d\n", current_weight, max_inventory_weight);
                    SLEEP(1000);
                    return;
                }
            }
        }
    }
    printf("주변에 어획 가능한 지점이 없습니다. 어획에 실패했습니다.\n");
    SLEEP(1000);
}



// 연료 충전을 시도하는 함수 (연료 섬 인접 시)
void refuel() {
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            int nx = ship_x + dx;
            int ny = ship_y + dy;
            if (nx >= 0 && nx < MAP_HEIGHT && ny >= 0 && ny < MAP_WIDTH) {
                if (map[current_layer][nx][ny].object == 'I') {
                    printf("(%d, %d) 근처에서 연료 충전 성공! 연료가 %d로 채워졌습니다.\n", nx, ny, myship.fuel2);
                    fuell = myship.fuel2;  // 연료를 채움
                    SLEEP(1000);
                    return;  // 연료 충전 후 함수 종료
                }
            }
        }
    }
    // 연료 충전 실패
    printf("주변에 연료 섬이 없습니다. 연료 충전에 실패했습니다.\n");
    SLEEP(1000);
}

void move_towards_target() {
    if (ship_x == target_x && ship_y == target_y) {
        printf("목표 지점에 도착했습니다!\n");
        SLEEP(1000);
        target_x = -1;
        target_y = -1;
        return;
    }

    if (myship.velocity >= 200) {
        map[current_layer][ship_x][ship_y].object = '\0';

        int distance = abs(target_x - ship_x) + abs(target_y - ship_y);
        ship_x = target_x;
        ship_y = target_y;

        fuell -= distance;

        map[current_layer][ship_x][ship_y].object = 'B';

        printf("속도가 너무 빨라 목표 지점으로 바로 이동합니다!\n");
        SLEEP(1000);
        return;
    }

    struct Node {
        int x, y;
        int parent_index;
    };

    Node queue[MAP_HEIGHT * MAP_WIDTH];
    int front = 0, rear = 0;
    Node nodes[MAP_HEIGHT * MAP_WIDTH];
    int visited[MAP_HEIGHT][MAP_WIDTH] = { 0 };

    queue[rear++] = { ship_x, ship_y, -1 };
    visited[ship_x][ship_y] = 1;

    bool found = false;
    int target_index = -1;

    while (front < rear) {
        Node current = queue[front++];
        nodes[front - 1] = current;

        if (current.x == target_x && current.y == target_y) {
            found = true;
            target_index = front - 1;
            break;
        }

        int directions[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
        for (int i = 0; i < 4; i++) {
            int new_x = current.x + directions[i][0];
            int new_y = current.y + directions[i][1];

            if (new_x >= 0 && new_x < MAP_HEIGHT && new_y >= 0 && new_y < MAP_WIDTH &&
                !visited[new_x][new_y] && map[current_layer][new_x][new_y].object != 'X' && map[current_layer][new_x][new_y].object != 'I') {
                visited[new_x][new_y] = 1;
                queue[rear++] = { new_x, new_y, front - 1 };
            }
        }
    }

    if (found) {
        Node current = nodes[target_index];
        while (current.parent_index != -1) {
            if (nodes[current.parent_index].x == ship_x && nodes[current.parent_index].y == ship_y) {
                break;
            }
            current = nodes[current.parent_index];
        }

        map[current_layer][ship_x][ship_y].object = '\0';

        ship_x = current.x;
        ship_y = current.y;

        fuell -= 1;

        map[current_layer][ship_x][ship_y].object = 'B';
        int sleep_time = 500 / myship.velocity;
        if (sleep_time > 0) {
            SLEEP(sleep_time);
        }
    }
    else {
        printf("목표 지점으로 가는 길이 없습니다!\n");
        SLEEP(1000);
        target_x = -1;
        target_y = -1;
    }
}

void reset_layer(int previous_layer) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (map[previous_layer][i][j].object == 'B') {
                map[previous_layer][i][j].object = '\0';
            }
        }
    }
}

void change_layer(char direction) {
    int temp = current_layer;  // 현재 레이어 값을 임시 변수에 저장

    if (direction == 'w' && ship_x == 0) {
        current_layer = (current_layer + 1) % NUM_LAYERS;
        ship_x = MAP_HEIGHT - 1;
    }
    else if (direction == 's' && ship_x == MAP_HEIGHT - 1) {
        current_layer = (current_layer + 1) % NUM_LAYERS;
        ship_x = 0;
    }
    else if (direction == 'a' && ship_y == 0) {
        current_layer = (current_layer + 1) % NUM_LAYERS;
        ship_y = MAP_WIDTH - 1;
    }
    else if (direction == 'd' && ship_y == MAP_WIDTH - 1) {
        current_layer = (current_layer + 1) % NUM_LAYERS;
        ship_y = 0;
    }

    // 이전 레이어 초기화 (어선의 흔적 제거)
    reset_layer(temp);

    // 새로운 레이어 초기화 (매번 다른 지형 생성)
    initialize_layer(current_layer);

    // 새로운 위치에 어선을 표시 (현재 레이어에 표시)
    map[current_layer][ship_x][ship_y].object = 'B';
}

// 어선을 이동시키는 함수
void move_ship(char direction) {
    if (fuell <= 0) {
        printf("연료가 다 떨어졌습니다! 게임 오버.\n");
        exit(0);  // 연료가 없으면 게임 종료
    }

    // 새로운 좌표를 계산
    int new_x = ship_x;
    int new_y = ship_y;

    switch (direction) {
    case 'w': // 위쪽으로 이동
        new_x--;
        if (new_x < 0) {
            change_layer('w');
            return;
        }
        break;
    case 's': // 아래쪽으로 이동
        new_x++;
        if (new_x >= MAP_HEIGHT) {
            change_layer('s');
            return;
        }
        break;
    case 'a': // 왼쪽으로 이동
        new_y--;
        if (new_y < 0) {
            change_layer('a');
            return;
        }
        break;
    case 'd': // 오른쪽으로 이동
        new_y++;
        if (new_y >= MAP_WIDTH) {
            change_layer('d');
            return;
        }
        break;
    default:
        printf("잘못된 방향입니다!\n");
        SLEEP(1000);
        return;
    }

    // 장애물 체크 (장애물 위치로 이동 불가)
    if (map[current_layer][new_x][new_y].object == 'X' || map[current_layer][new_x][new_y].object == 'I') {
        printf("앞에 장애물이 있습니다! 그쪽으로 이동할 수 없습니다.\n");
        SLEEP(1000);
        return;
    }

    // 현재 위치의 상태 복원 (레이어에서 어선 제거)
    map[current_layer][ship_x][ship_y].object = '\0';

    // 이동한 좌표 업데이트
    ship_x = new_x;
    ship_y = new_y;

    // 연료 소모
    fuell -= (map[current_layer][ship_x][ship_y].terrain == 'C') ? 2 : 1;

    // 새로운 위치에 어선을 표시 (레이어에 표시)
    map[current_layer][ship_x][ship_y].object = 'B';
}
int main() {
    // 게임 시작 시 저장된 데이터를 불러옵니다.
    load_game_data();

    fuell = myship.fuel2;  // 불러온 데이터로 연료 초기화
    system("mode con:cols=500 lines=50");
    show_intro();
    char command;

    // 콘솔 핸들 초기화
    consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    // 맵을 초기화
    initialize_map();

    while (1) {
        // 맵을 출력
        print_map();

        // 목표 좌표가 설정된 경우 목표 지점으로 이동
        if (target_x != -1 && target_y != -1) {
            move_towards_target();
            SLEEP(500);
            continue;
        }

        // 명령 입력
        printf("현재 골드: %d\n", gold);
        printf("명령 입력 (움직이려면 w/a/s/d를, 낚시를 하려면 f를, 연료를 충전하려면 k를, 타겟을 정해 움직이려면 t를, 게임을 끝내려면 q를, 상점에 들어가려면 b를 누르세요): ");
        command = _getch();

        if (command == 'q') {
            // 게임 종료 전에 데이터 저장
            save_game_data();
            printf("게임이 종료되었습니다. 데이터가 저장되었습니다.\n");
            break;
        }
        else if (command == 'b') {
            enter_shop();
            save_game_data();  // 상점에서의 구매나 업그레이드 후 데이터 저장
        }
        else if (command == 'f') {
            start_fishing();
        }
        else if (command == 'k') {
            refuel();
        }
        else if (command == 't') {
            while (1) {
                printf("\nEnter target coordinates (x y): ");
                int first_char = _getch();
                if (first_char >= 48 && first_char <= 57) {  // Check if first input is a number (ASCII 48-57)
                    ungetc(first_char, stdin);  // Put the character back to stdin
                    scanf("%d %d", &target_x, &target_y);
                    if (target_x < 0 || target_x >= MAP_HEIGHT || target_y < 0 || target_y >= MAP_WIDTH) {
                        printf("Invalid coordinates!\n");
                        target_x = -1;
                        target_y = -1;
                    }
                    else {
                        print_map();  // 목표 지점을 즉시 표시
                        break;
                    }
                }
                else {
                    printf("Invalid input! Please enter valid coordinates.\n");
                }
            }
        }
        else {
            move_ship(command); // w/a/s/d 입력 시 어선 이동
        }

        // 주기적으로 데이터 저장을 제거하여 성능 향상
    }

    return 0;
}