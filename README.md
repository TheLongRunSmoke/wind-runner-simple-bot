# wind-runner-simple-bot
======================
Простой бот для андроид-версии игры LINE Wind Runner.

Написал на досуге. Умеет следить за количеством попыток и делать "слепые забеги". 
В игре ограничено количество ресурсов получаемых за одну попытку, поэтому 
имея среднего персонажа можно получить возможный максимум не делая ничего.

Написано на С++, с использованием библиотеки машинного зрения OpenCV и 
утилиты adb(Android Debug Bridge).

Как использовать.
1. Создайте проект консольного приложения в Visual Studio.
2. Настройте проект для сборки с OpenCV. Использована версия 2.4.10.
3. Если разрешение экрана вашего устройства отличается от 540 x 960
  создайте новые шаблоны, смотрите папку roi. Либо добавьте нормализацию.
4. Поправьте коорднаты точек в пакетных файлах в папке adb.
5. Соберите.
...
6. Profit!)

По всем вопросам можно обращатся ко мне thelongrunsmoke@gmail.com.

Версия устарела.
