
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


void f1() {
    A = 2;
    B = B + 5;
}

void f2() {
    A = A * 2;
    B = B + C;
}

void f3() {
    C = B + 10;
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


int main()
{
    int zadanie = 1;
    if (zadanie == 1) {
        std::cout << "---------------------------------Zadanie 1---------------------------------" << std::endl;
        /*
        * Zadanie 1: Zdefiniowane zosta�y 3 zmienne globalne (A, B, C), na kt�rych operuj� trzy funkcje (f1, f2, f3).
        * Za pomoc� semafor�w zsynchronizuj w�tki tak, aby suma wspomianych wcze�niej zmiennych globalnych wynosi�a 37.
        */
        std::jthread jt1(f1);
        std::jthread jt2(f2);
        std::jthread jt3(f3);

        std::cout << "Suma: " << A + B + C << std::endl;

    }
    
    if (zadanie == 2) {
        std::cout << "---------------------------------Zadanie 2---------------------------------" << std::endl;
        /*
        * Zadanie 2: Stworzone zosta�o wyra�enie lambda "licz" oraz vector w�tk�w, w kt�rych jest ono wykonywane.
        * Do zrealizowania zadania wykorzystaj barier�.
        * 2.1 Stw�rz wyra�enie lambda i wypisz w nim maksymaln� otrzyman� warto�� (std::max_element).
        * 2.2 Wykorzystaj stworzon� poni�ej p�tl� (1) umo�liwaj�c� wy�wietlenie paru najwi�kszych warto�ci. Nie zmieniaj jej po�o�enia.
        */
        std::vector<int> vec(100);
        std::iota(vec.begin(), vec.end(), 100);
        std::vector<std::jthread> jtvec;



        auto licz = [&](int& a) {
            for (int i = 0; i < 1000; i++) {
                a -= 2;
            }
            
        };
        for (int i = 0; i < 100; i++) {
            jtvec.push_back(std::jthread(licz, std::ref(vec.at(i))));
        }
       
        

        for (int i = 0; i < 100; i++) { //(1)
            if (vec.at(i) >= -1805)
                std::cout << vec.at(i) << std::endl;
        }
    }

    if (zadanie == 3) {
        std::cout << "---------------------------------Zadanie 3---------------------------------" << std::endl;
        /*
        * Zadanie 3: Tre�� podobna do tej powy�ej, lecz z wykorzystaniem counting_semaphore.
        * Stworzone zosta�o wyra�enie lambda "licz" oraz vector w�tk�w w kt�rych jest ono wykonywane.
        * Do zrealizowania zadania wykorzystaj counting_semaphore.
        * 3.1 Wypisz maksymaln� otrzyman� warto�� (std::max_element).
        * 3.2 Wykorzystaj stworzon� poni�ej p�tl� (1) umo�liwaj�c� wy�wietlenie paru najwi�kszych warto�ci. Nie zmieniaj jej po�o�enia.
        */
        std::vector<int> vec(100);
        std::iota(vec.begin(), vec.end(), 100);
        std::vector<std::jthread> jtvec;
        //deklaracje

        auto licz = [&](int& a) {
            for (int i = 0; i < 1000; i++) {
                a -= 2;
            }
            
        };
        for (int i = 0; i < 100; i++) {
            jtvec.push_back(std::jthread(licz, std::ref(vec.at(i))));
        }

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
        * W pewnej restauracji, ze wzgl�du na brak wi�kszej ilo�ci miejsc, obiad mo�e je�� wy��cznie 6 go�ci jednocze�nie.
        * Obiad chce zje�� 30 klient�w, lecz z uwagi na pi�kn� pogod�, nie staj� oni w kolejce, a co jaki� czas sprawdzaj�, czy nie zwolni�o si� jakie� miejsce.
        * Jednak, ze wzgl�du na p�n� por�, restauracja jest w stanie obs�u�y� jeszcze wy��cznie 20 klient�w, pozostali musz� obej�� si� ze smakiem.
        * Zjedzenie obiadu, brak dost�pnych w danej chwili miejsc i to, �e dany klient nie da� rady zje�� tego dnia obiadu, jest symulowane odpowiednim komunikatem.
        * 4.1 Za pomoc� counted_semaphore i metody try_acquire umo�liw klientowi sprawdzenie, czy w restauracji jest wolne miejsce.
        * 4.2 Obiadu nie zjad�o 10 klient�w. Za pomoc� bariery spraw, by komunikaty o tym informuj�ce pojawi�y si� na samym ko�cu.
        */
        std::vector<std::jthread> jtvec;
        for (int i = 0; i < 30; i++) {
            jtvec.emplace_back(klient, i+1);
        }
        
        
    }
    return 0;
}


//Deklaracje z zadania 4 zr�b przed mainem.

void klient(int nr) {
    bool zjadl = false;
    while (IloscMiejscNaDzis > 0 && !zjadl) {
        if (0/*zmodyfikuj*/) { //sprawdzenie, czy w restauracji jest wolne miejsce
            IloscMiejscNaDzis--;
            Sleep(distrib(gen));
            mut.lock();
            std::cout << "Klientowi numer " << nr << " udalo sie zjesc obiad!" << std::endl;
            mut.unlock();
            zjadl = true;
        }
        else { //brak wolnego miejsca
            Sleep(distrib(gen));
            mut.lock();
            std::cout << "Podejscie klienta numer " << nr << " skonczylo sie niepowodzeniem!" << std::endl;
            mut.unlock();
        }
    }
    if (!zjadl) { //nie uda�o si� zje�� obiadu
        mut.lock();
        std::cout << "Klientowi numer " << nr << " dzisiaj nie udalo sie zjesc obiadu!" << std::endl;
        mut.unlock();
    }
}