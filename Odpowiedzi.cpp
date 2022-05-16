
#include <iostream>
#include <vector>
#include <semaphore>
#include <thread>
#include <barrier>
#include <string>
#include <windows.h>
#include <numeric>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <random>

//Do zadania 1, tutaj niczego nie zmieniaj
int A = 0, B = 0, C = 0;
//Tutaj deklaracje z zadania 1:
std::binary_semaphore a{ 0 }, b{ 1 }, c{ 0 }, d{ 0 };

void f1() {
    a.acquire();
    A = 2;
    B = B + 5;
    c.release();
}

void f2() {
    b.acquire();
    A = A * 2;
    a.release();
    b.acquire();
    B = B + C;
    d.release();
}

void f3() {
    c.acquire();
    C = B + 10;
    b.release();
    c.acquire();
    B = 6;
}

//Do zadania 4, tutaj niczego nie zmieniaj
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution distrib(500, 700);
std::atomic<int> IloscMiejscNaDzis(20);
std::mutex mut;
void klient(int nr);
//Tutaj deklaracje do zadania 4:
std::counting_semaphore<6> cs(0);
std::barrier bz4(30);

int main()
{
    int zadanie = 1;
    if (zadanie == 1) {
        std::cout << "---------------------------------Zadanie 1---------------------------------" << std::endl;
        /*
        * Zadanie 1: Zdefiniowane zostały 3 zmienne globalne (A, B, C), na których operują trzy funkcje (f1, f2, f3).
        * Za pomocą semaforów zsynchronizuj wątki tak, aby suma wspomianych wcześniej zmiennych globalnych wynosiła 37.
        */
        std::jthread jt1(f1);
        std::jthread jt2(f2);
        std::jthread jt3(f3);
        d.acquire();
        std::cout << "Suma: " << A + B + C << std::endl;
        b.release();
        c.release();
    }
    
    if (zadanie == 2) {
        std::cout << "---------------------------------Zadanie 2---------------------------------" << std::endl;
        /*
        * Zadanie 2: Stworzone zostało wyrażenie lambda "licz" oraz vector wątków, w których jest ono wykonywane.
        * Do zrealizowania zadania wykorzystaj barierę.
        * 2.1 Stwórz wyrażenie lambda i wypisz w nim maksymalną otrzymaną wartość (std::max_element).
        * 2.2 Wykorzystaj stworzoną poniżej pętlę (1) umożliwającą wyświetlenie paru największych wartości. Nie zmieniaj jej położenia.
        */
        std::vector<int> vec(100);
        std::iota(vec.begin(), vec.end(), 100);
        std::vector<std::jthread> jtvec;

        std::binary_semaphore s{ 0 };

        auto comp = [&]() noexcept { 
            int m = *std::max_element(vec.begin(), vec.end());
            std::cout << "Maksymalny element: " << m << std::endl;
            s.release(); //Oczywiście pętlę (1) można by było wstawić tutaj i nie używać semafora.
        };

        std::barrier sync(100, comp);
        auto licz = [&](int& a) {
            for (int i = 0; i < 1000; i++) {
                a -= 2;
            }
            sync.arrive_and_wait();
        };
        for (int i = 0; i < 100; i++) {
            jtvec.push_back(std::jthread(licz, std::ref(vec.at(i))));
        }
       
        s.acquire();

        for (int i = 0; i < 100; i++) { //(1)
            if (vec.at(i) >= -1805)
                std::cout << vec.at(i) << std::endl;
        }
    }

    if (zadanie == 3) {
        std::cout << "---------------------------------Zadanie 3---------------------------------" << std::endl;
        /*
        * Zadanie 3: Treść podobna do tej powyżej, lecz z wykorzystaniem counting_semaphore.
        * Stworzone zostało wyrażenie lambda "licz" oraz vector wątków w których jest ono wykonywane.
        * Do zrealizowania zadania wykorzystaj counting_semaphore.
        * 3.1 Wypisz maksymalną otrzymaną wartość (std::max_element).
        * 3.2 Wykorzystaj stworzoną poniżej pętlę (1) umożliwającą wyświetlenie paru największych wartości. Nie zmieniaj jej położenia.
        */
        std::vector<int> vec(100);
        std::iota(vec.begin(), vec.end(), 100);
        std::vector<std::jthread> jtvec;
        //deklaracje
        std::counting_semaphore<100> sync(0);
        auto licz = [&](int& a) {
            for (int i = 0; i < 1000; i++) {
                a -= 2;
            }
            sync.release();
        };
        for (int i = 0; i < 100; i++) {
            jtvec.push_back(std::jthread(licz, std::ref(vec.at(i))));
        }
        for (int i = 0; i < 100; i++)
            sync.acquire();
        int m = *std::max_element(vec.begin(), vec.end());
        std::cout << "Maksymalny element: " << m << std::endl;
        for (int i = 0; i < 100; i++) { //(1)
            if (vec.at(i) >= -1805)
                std::cout << vec.at(i) << std::endl;
        }
    }

    if (zadanie == 4) {
        std::cout << "---------------------------------Zadanie 4---------------------------------" << std::endl;
        /* Zadanie 4
        * W pewnej restauracji, ze względu na brak większej ilości miejsc, obiad może jeść wyłącznie 6 gości jednocześnie.
        * Obiad chce zjeść 30 klientów, lecz z uwagi na piękną pogodę, nie stają oni w kolejce, a co jakiś czas sprawdzają, czy nie zwolniło się jakieś miejsce.
        * Jednak, ze względu na późną porę, restauracja jest w stanie obsłużyć jeszcze wyłącznie 20 klientów, pozostali muszą obejść się ze smakiem.
	* Zjedzenie obiadu, brak dostępnych w danej chwili miejsc i to, że dany klient nie dał rady zjeść tego dnia obiadu, jest symulowane odpowiednim komunikatem.
        * 4.1 Za pomocą counted_semaphore i metody try_acquire umożliw klientowi sprawdzenie, czy w restauracji jest wolne miejsce.
        * 4.2 Obiadu nie zjadło 10 klientów. Za pomocą bariery spraw, by komunikaty o tym informujące pojawiły się na samym końcu.
        */
        std::vector<std::jthread> jtvec;
        for (int i = 0; i < 30; i++) {
            jtvec.emplace_back(klient, i+1);
        }
        cs.release(6);
        
    }
    return 0;
}


//Deklaracje z zadania 4 zrób przed mainem.

void klient(int nr) {
    bool zjadl = false;
    while (IloscMiejscNaDzis > 0 && !zjadl) {
        if (cs.try_acquire()) { //sprawdzenie, czy w restauracji jest wolne miejsce
            IloscMiejscNaDzis--;
            Sleep(distrib(gen));
            mut.lock();
            std::cout << "Klientowi numer " << nr << " udalo sie zjesc obiad!" << std::endl;
            mut.unlock();
            zjadl = true;
            cs.release();
            bz4.arrive_and_wait();
        }
        else { //brak wolnego miejsca
            Sleep(distrib(gen));
            mut.lock();
            std::cout << "Podejscie klienta numer " << nr << " skonczylo sie niepowodzeniem!" << std::endl;
            mut.unlock();
        }
    }
    if (!zjadl) { //nie udało się zjeść obiadu
        bz4.arrive_and_wait();
        mut.lock();
        std::cout << "Klientowi numer " << nr << " dzisiaj nie udalo sie zjesc obiadu!" << std::endl;
        mut.unlock();
    }
}
