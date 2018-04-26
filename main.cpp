/*
    меню загрузки
    author Трифонов Александр (ОКВ СФЗ) truetrifonov@yandex.ru
*/

#include <iostream>
#include <locale.h>
#include <pthread.h>

#include "cfg.h"

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#define FSEP "\\"
#endif

#ifdef __linux__

#include <unistd.h>
#include "mygetch.h"

#define FSEP "/"
#endif

#define MAINCFG "cfg.cfg"
#define MENUCFG "menu.cfg"
#define KEYMENU 10        // код кнопки вызова меню
#define KEYUP 119      // код кнопки ВВЕРХ
#define KEYDOWN 115        // код кнопки ВНИЗ
#define KEYSELECT 10    // код кнопки ВЫБОР


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
void clear()
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
    clear();

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
    clear();
    int index = std::stoi(menu["@"]);
    int counter = 0;

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

    setlocale(0, "rus");

    // определяем текущий каталог
    char *catalog = getcwd(NULL, 0);
    if (catalog == NULL)
    {
        perror("ERROR: Ошибка getcwd()");
        return 0;
    }

    // читаем и устанавливаем основные параметры
    snprintf(cfgmain, 1000, "%s%s%s", catalog, FSEP, MAINCFG);
    cfg config(cfgmain);
    time = std::stoi(config["wait"]);

    // читаем содержание меню
    snprintf(cfgmenu, 1000, "%s%s%s", catalog, FSEP, MENUCFG);
    cfg menu(cfgmenu);

    if(!menu.check())
        printf("\n%s is wrong\n", MENUCFG);
    else
        clear();

    // запускаю поток обработки нажатия кнопки
    pthread_create(&thread, NULL, thread_waitkey, &key);

    // ожидание действий пользователя
    int timer = 0;
    while (key == 0 && timer < time)
    {
        wait();
        timer++;
    }

    // пост-обработка
    if (key == 0)
    {
        //pthread_cancel(thread);
        run(menu.getSelValue());
        return 0;
    }

    // отрисока меню и обработка интерфейса
    drawMenu(menu);
    for (;;)
    {
        int index;
        switch (getch())
        {
            case KEYUP:
                index = std::stoi(menu["@"]);
                if (index > 0 && index < menu.count)
                    menu.setSelIndex(index - 1);
                else
                {
                    menu.setSelIndex(0);
                    std::cout << "\a";
                }

                drawMenu(menu);
                break;

            case KEYDOWN:
                index = std::stoi(menu["@"]);
                if (index > -1 && index < menu.count - 1)
                    menu.setSelIndex(index + 1);
                else
                {
                    menu.setSelIndex(menu.count - 1);
                    std::cout << "\a";
                }

                drawMenu(menu);
                break;

            case KEYSELECT:
                menu.save(cfgmenu);
                run(menu.getSelValue());
                return 0;

            default:
                std::cout << "\a";
                break;
        }
    }
}





