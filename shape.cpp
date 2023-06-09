#include "shape.h"
// ПРИМЕР ДОБАВКИ: дополнительный фрагмент - полуокружность
class h_circle: public rectangle, public reflectable {
    bool reflected;
public:
    h_circle(point a, point b, bool r=true) : rectangle(a, b), reflected(r) { }
    void draw();
    void flip_horisontally( ) { }; // Отразить горизонтально (пустая функция)
    void flip_vertically( ) { reflected = !reflected; };	// Отразить вертикально
};
void h_circle :: draw() //Алгоритм Брезенхэма для окружностей
{   //(выдаются два сектора, указываемые значением reflected)
    int x0 = (sw.x + ne.x)/2, y0 = reflected ? sw.y : ne.y;
    int radius = (ne.x - sw.x)/2;
    int x = 0, y = radius, delta = 2 - 2 * radius, error = 0;
    while(y >= 0) { // Цикл рисования
        if(reflected) { put_point(x0 + x, y0 + y*0.7); put_point(x0 - x, y0 + y*0.7); }
        else { put_point(x0 + x, y0 - y*0.7); put_point(x0 - x, y0 - y*0.7); }
        error = 2 * (delta + y) - 1;
        if(delta < 0 && error <= 0) { ++x; delta += 2 * x + 1; continue; }
        error = 2 * (delta - x) - 1;
        if(delta > 0 && error > 0) { --y; delta += 1 - 2 * y; continue; }
        ++x; delta += 2 * (x - y);  --y;
    }
}
// ПРИМЕР ДОБАВКИ: дополнительная функция присоединения…
void down(shape &p,  const shape &q)
{    point n = q.south( );
    point s = p.north( );
    p.move(n.x - s.x, n.y - s.y - 1); }
// Сборная пользовательская фигура - физиономия
class myshape : public rectangle { // Моя фигура ЯВЛЯЕТСЯ
    int w, h;			             //        прямоугольником
    line l_eye; // левый глаз – моя фигура СОДЕРЖИТ линию
    line r_eye; // правый глаз
    line mouth; // рот
public:
    myshape(point, point);
    void draw( );
    void move(int, int);
    void resize(int) { }
};
myshape :: myshape(point a, point b)
        : rectangle(a, b),	//Инициализация базового класса
          w(neast( ).x - swest( ).x + 1), // Инициализация данных
          h(neast( ).y - swest( ).y + 1), // - строго в порядке объявления!
          l_eye(point(swest( ).x + 2, swest( ).y + h * 3 / 4), 2),
          r_eye(point(swest( ).x + w - 4, swest( ).y + h * 3 / 4), 2),
          mouth(point(swest( ).x + 2, swest( ).y + h / 4), w - 4)
{ }
void myshape :: draw( )
{
    rectangle :: draw( ); //Контур лица (глаза и нос рисуются сами!)
    int a = (swest( ).x + neast( ).x) / 2;
    int b = (swest( ).y + neast( ).y) / 2;
    put_point(point(a, b)); // Нос – существует только на рисунке!
}
void myshape :: move(int a, int b)
{
    rectangle :: move(a, b);
    l_eye.move(a, b);  r_eye.move(a, b);
    mouth.move(a, b);
}

int main(){
    setlocale(LC_ALL, "Rus");
    screen_init();

//    Ошибка 1 - Перемещение за экран
//    crossedCircle err(point(0, 0), point(10, 10));
//    shape_refresh( );
//    std::cout << "\n";
//    err.move(-1, -1);
//    shape_refresh( );
//    std::cout << "\n";

//    Ошибка 2 - Помещение за экран
//    crossedCircle err(point(-3, 2), point(10, 5));
//    shape_refresh( );
//    std::cout << "\n";

//    Ошибка 3 - Плохие входные точки
    crossedCircle err(point(10, 0), point(0, 10));
    shape_refresh( );
    std::cout << "\n";

//== 1.Объявление набора фигур ==
    rectangle hat(point(0, 0), point(14, 5));
    line brim(point(0,15),17);
    myshape face(point(15,10), point(27,18));
    h_circle beard(point(40,10), point(50,20));
    crossedCircle A(point(33,30), point(37,26));
    crossedCircle B(point(16, 44), point(26, 34));
    crossedCircle C(point(5, 33), point(12, 26));
    crossedCircle D(point(18, 11), point(24, 1));
    shape_refresh( );
    std::cout << "=== Generated... ===\n";
    std::cin.get(); //Смотреть исходный набор
//== 2.Подготовка к сборке ==
    hat.rotate_right( );
    brim.resize(2);
    face.resize(2);
    beard.flip_vertically();
    shape_refresh( );
    std::cout << "=== Prepared... ===\n";
    std::cin.get(); //Смотреть результат поворотов/отражений
//== 3.Сборка изображения ==
//	face.move(0, -10); // Лицо - в исходное положение
    up(brim, face);
    up(hat, brim);
    down(beard, face);
    hat.move(0, 6);
    brim.move(0, 6);
    face.move(0, 6);
    beard.move(0, 6);
    shape_refresh( );
    std::cout << "=== Ready! ===\n";
    std::cin.get(); //Смотреть результат
    screen_destroy( );
    return 0;
}
