//Размер экрана
const int XMAX=120;
const int YMAX=50;
//Точка на экране
class point {
public:
    int x, y;
    point(int a = 0, int b = 0) : x(a), y(b) {}
};
// Набор утилит для работы с экраном
// Вывод точки (2 варианта)
void put_point(int, int);
void put_point(point p) {
    put_point(p.x, p.y);
}
// Вывод линии (2 варианта)
void put_line(int, int, int, int);
void put_line(point a, point b) {
    put_line(a.x, a.y, b.x, b.y);
}
// Создание экрана
void screen_init();
// Удаление
void screen_destroy();
// Обновление
void screen_refresh();
// Очистка
void screen_clear();