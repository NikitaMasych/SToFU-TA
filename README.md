Тестове завдання: засобами STL та WinAPI реалізувати консольну програму, яка на вхід приймає повний шлях до файлу .ехе та файлу .ico.

Данна консольна програма:

* Перераховує всі імена бібліотек і WinAPI, які є в таблиці імпорту .ехе файла, і виводить їх у консоль;
* Рахує кількість імен WinAPI, які містять літеру 'W' в таблиці імпорта .ехе;
* Замінює основну іконку у файлі .ехе на ту, яка була передана консольному додатку через аргумент;
* Рахує інформаційну ентропію .ехе файла та виводить її у консоль;
* Рахує інформаційну ентропію .ico файла та виводить її у консоль.

** Warning: with some .EXE files need to use x64 architecture program build, with some - x86 due to peculiarities of the .PE build **

** Entropy calculation may take some time, because I didn't use external boost mmap for fast reading, regarding to the assignment specifications **
