/*
    МЕНЮ ЗАГРУЗКИ
    Трифонов Александр (ОКВ СФЗ) truetrifonov@yandex.ru
    ---------------------------------------------------
    CFGMAIN - файл конфигурации программы
    SFGMENU - файл конфигурации меню
    Программа ждёт "wait" секунд, которые указаны в CFGMAIN
    В течении указанного времени нужно нажать клавишу KEYMENU
    После этого отображается меню загрузки, которое настраивается в CFGMENU
    При помощи клавиш key_up и key_down (CFGMAIN) производится выбор,
    key_select (CFGMAIN) - клавиша подтверждения
    После подтверждения запускается на выполнение выбранная команда, 
    CFGMAIN переписывается
*/

#include <fstream>
#include <iostream>
#include <locale.h>
#include <pthread.h>

#include "cfg.h"

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#define FSEP "\\"           // разделитель в фс
#endif

#ifdef __linux__
#include <unistd.h>
#include "mygetch.h"
#define FSEP "/"            // разделитель в фс
#endif

#define CFGMAIN "cfg.cfg"   // файл конфигурации программы
#define CFGMENU "menu.cfg"  // файл конфигурации меню
#define KEYMENU 10          // код кнопки вызова меню

// ожидание нажатия условленной клавиши из отдельного потока
void *thread_waitkey(void *yes)
{
    char character = 0;
    character = getch();

    //printf("\nkey pressed: %d\n", character);
    if (character == KEYMENU)
        *(int *) yes = 1;

    pthread_exit(NULL);
}

// ожидание 1 сек
void wait()
{
#ifdef _WIN32
    Sleep(1000);
#endif

#ifdef __linux__
    sleep(1);
#endif
}

// очистка экрана
void clearScreen()
{
#ifdef _WIN32
    system("cls");
#endif

#ifdef __linux__
    system("clear");
#endif
}

// выполнить
void run(string cmd)
{
    int ret = -1;
    clearScreen();

#ifdef _WIN32
    ret = system(cmd.c_str());
#endif

#ifdef __linux__
    ret = system(cmd.c_str());
#endif

    std::cout << "\nResult: " << ret << "\n";
}

// отрисовка меню
void drawMenu(cfg menu)
{
    int index = std::stoi(menu["@"]);
    int counter = 0;
    
    clearScreen();

    printf("+%s[MENU]%s+\n", string(15, '-').c_str(), string(15, '-').c_str());
    for (auto it = menu.begin(); it != menu.end(); ++it)
    {
        if (it->first != "" && it->first != "@")
        {
            if (counter == index)
                printf(" %s\t<\n", it->first.c_str());
            else
                printf("  %s\n", it->first.c_str());

            counter++;
        }
    }
    printf("+%s+\n", string(36, '-').c_str());
    printf("%s\n", menu.getSelValue().c_str());
}

// проверка наличия файла
bool isFileExist(char *filename)
{
    bool exist = false;
    std::ifstream in(filename);
 
    if(in.is_open())
        exist = true;
 
    in.close();
    return exist;
}

/////////////////////////////////////////////////////////////////
// main
/////////////////////////////////////////////////////////////////
int main()
{
    int key = 0;        // флаг нажатия условленной клавиши
    int time = 0;       // время ожидания нажатия кнопки
    pthread_t thread;   // поток обработки нажатия кнопки
    char cfgmain[1000]; // путь до основного конфига
    char cfgmenu[1000]; // путь до конфига меню
    int keyup;          // клавиша выбора вверх
    int keydown;        // клавиша выбора вниз
    int keyselect;      // клавиша подтверждения выбора

    setlocale(0, "rus");

    // определяем текущий каталог
    char *catalog = getcwd(NULL, 0);
    if (catalog == NULL)
    {
        perror("ERROR: getcwd()");
        return 1;
    }

    // читаем и устанавливаем основные параметры
    snprintf(cfgmain, 1000, "%s%s%s", catalog, FSEP, CFGMAIN);
    if(!isFileExist(cfgmain))
    {
        printf("\nFile '%s' not found\n", CFGMAIN);
        return 1;
    }
    cfg config(cfgmain);
    time = std::stoi(config["wait"]);

    // читаем содержание меню
    snprintf(cfgmenu, 1000, "%s%s%s", catalog, FSEP, CFGMENU);
    if(!isFileExist(cfgmenu))
    {
        printf("\nFile '%s' not found\n", CFGMENU);
        return 1;
    }
    cfg menu(cfgmenu);

    // проверка меню
    if(!menu.check())
        printf("\n%s is wrong\n", CFGMENU);
    else
        clearScreen();

    // запускаю поток обработки нажатия кнопки
    pthread_create(&thread, NULL, thread_waitkey, &key);

    // ожидание действий пользователя
    int timer = 0;
    while (key == 0 && timer < time)
    {
        wait();
        timer++;
    }

    // реакция на действия пользователя
    if (key == 0)
    {
        run(menu.getSelValue());
        return 0;
    }

    // отрисока меню и обработка ввода пользователя
    drawMenu(menu);
    try 
    {
        keyup = std::stoi(config["key_up"]);
        keydown = std::stoi(config["key_down"]);
        keyselect = std::stoi(config["key_select"]);
    }
    catch (std::invalid_argument) 
    {
        printf("\n%s is wrong\n", CFGMENU);
        std::cout << '\a';
        return 1;
    }
    for (;;)
    {
        int index;
        int key = getch();
        if (key == keyup)           // нажата кнопка ВВЕРХ
        {
            index = std::stoi(menu["@"]);
            if (index > 0 && index < menu.count)
                menu.setSelIndex(index - 1);
            else
            {
                menu.setSelIndex(0);
                std::cout << '\a';
            }

            drawMenu(menu);
        }
        else if (key == keydown)    // нажата кнопка ВНИЗ
        {
            index = std::stoi(menu["@"]);
            if (index > -1 && index < menu.count - 1)
                menu.setSelIndex(index + 1);
            else
            {
                menu.setSelIndex(menu.count - 1);
                std::cout << '\a';
            }

            drawMenu(menu);
        }
        else if(key == keyselect)   // нажата кнопка выбора
        {
            menu.save(cfgmenu);
            run(menu.getSelValue());
            return 0;
        }
        else                        // нажата неправильная кнопка
        {
            std::cout << '\a';
        }
    }
}





