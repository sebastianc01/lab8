
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
        * Zadanie 1: Zdefiniowane zosta³y 3 zmienne globalne (A, B, C), na których operuj¹ trzy funkcje (f1, f2, f3).
        * Za pomoc¹ semaforów zsynchronizuj w¹tki tak, aby suma wspomianych wczeœniej zmiennych globalnych wynosi³a 37.
        */
        std::jthread jt1(f1);
        std::jthread jt2(f2);
        std::jthread jt3(f3);
        d.acquire();
        std::cout << "Suma: " << A + B + C << std::endl;
        b.release();
        c.release();
        std::cout << distrib(gen) << distrib(gen);
    }
    
    if (zadanie == 2) {
        std::cout << "---------------------------------Zadanie 2---------------------------------" << std::endl;
        /*
        * Zadanie 2: Stworzone zosta³o wyra¿enie lambda "licz" oraz vector w¹tków, w których jest ono wykonywane.
        * Do zrealizowania zadania wykorzystaj barierê.
        * 2.1 Stwórz wyra¿enie lambda i wypisz w nim maksymaln¹ otrzyman¹ wartoœæ (std::max_element).
        * 2.2 Wykorzystaj stworzon¹ poni¿ej pêtlê (1) umo¿liwaj¹c¹ wyœwietlenie paru najwiêkszych wartoœci. Nie zmieniaj jej po³o¿enia.
        */
        std::vector<int> vec(100);
        std::iota(vec.begin(), vec.end(), 100);
        std::vector<std::jthread> jtvec;

        std::binary_semaphore s{ 0 };

        auto comp = [&]() noexcept { 
            int m = *std::max_element(vec.begin(), vec.end());
            std::cout << "Maksymalny element: " << m << std::endl;
            s.release(); //Oczywiœcie pêtlê (1) mo¿na by by³o wstawiæ tutaj i nie u¿ywaæ semafora.
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
        * Zadanie 3: Treœæ podobna do tej powy¿ej, lecz z wykorzystaniem counting_semaphore.
        * Stworzone zosta³o wyra¿enie lambda "licz" oraz vector w¹tków w których jest ono wykonywane.
        * Do zrealizowania zadania wykorzystaj counting_semaphore.
        * 3.1 Wypisz maksymaln¹ otrzyman¹ wartoœæ (std::max_element).
        * 3.2 Wykorzystaj stworzon¹ poni¿ej pêtlê (1) umo¿liwaj¹c¹ wyœwietlenie paru najwiêkszych wartoœci. Nie zmieniaj jej po³o¿enia.
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
        * W pewnej restauracji, ze wzglêdu na brak wiêkszej iloœci miejsc, obiad mo¿e jeœæ wy³¹cznie 6 goœci jednoczeœnie.
        * Obiad chce zjeœæ 30 klientów, lecz z uwagi na piêkn¹ pogodê, nie staj¹ oni w kolejce, a co jakiœ czas sprawdzaj¹, czy nie zwolni³o siê jakieœ miejsce.
        * Jednak, ze wzglêdu na póŸn¹ porê, restauracja jest w stanie obs³u¿yæ jeszcze wy³¹cznie 20 klientów, pozostali musz¹ obejœæ siê ze smakiem.
        * 4.1 Za pomoc¹ counted_semaphore i metody try_acquire umo¿liw klientowi sprawdzenie, czy w restauracji jest wolne miejsce.
        * 4.2 Obiadu nie zjad³o 10 klientów. Za pomoc¹ bariery spraw, by komunikaty o tym informuj¹ce pojawi³y siê na samym koñcu.
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
    if (!zjadl) { //nie uda³o siê zjeœæ obiadu
        bz4.arrive_and_wait();
        mut.lock();
        std::cout << "Klientowi numer " << nr << " dzisiaj nie udalo sie zjesc obiadu!" << std::endl;
        mut.unlock();
    }
}