#include "scaliansudoku.h"
#include "ui_scaliansudoku.h"
#include <QPixmap>
#include <QDebug>

ScalianSudoku::ScalianSudoku(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ScalianSudoku)
    , sudokuVacio{true}
{
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    ui->setupUi(this);

    ui->FrameCeldas->setVisible(false);

    ui->LogoScalian->setPixmap(QPixmap(":/logo/scalian"));
    ui->LogoCampus->setPixmap(QPixmap(":/logo/campus42"));
    ui->LogoCampus->setScaledContents(true);

    int itemIdx = 0;
    uint filas = ui->Tablero->count();
    for(uint filaId = 0; filaId < filas; filaId++)
    {
        if(auto widget = ui->Tablero->itemAt(filaId)->widget())
        {
            if(widget->objectName().contains("separator"))
            {
                continue;
            }
        }
        auto fila = ui->Tablero->itemAt(filaId)->layout();
        uint celdas = fila->count();
        for(uint celdaId = 0; celdaId < celdas; celdaId++)
        {
            auto celda = dynamic_cast<QLabel*>(fila->itemAt(celdaId)->widget());
            if(not celda)
            {
                continue;
            }

            celda->setText("");
            celda->setProperty("fila", itemIdx/9);
            celda->setProperty("col", itemIdx%9);
            celda->installEventFilter(this);
            itemIdx++;
        }
    }

    connect(ui->botonLimpiar, &QPushButton::clicked, this, &ScalianSudoku::onLimpiarSudoku);
    connect(ui->botonResolver, &QPushButton::clicked, this, &ScalianSudoku::onResolverSudoku);

    connect(ui->Aceptar, &QPushButton::clicked, this, &ScalianSudoku::onAceptar);
    connect(ui->Cancelar, &QPushButton::clicked, this, &ScalianSudoku::onCancelar);
    connect(ui->Borrar, &QPushButton::clicked, this, &ScalianSudoku::onBorrar);
}

void ScalianSudoku::limpiarSudoku()
{
    for(uint filaId = 0; filaId < 9; filaId++)
    {
        for(uint colId = 0; colId < 9; colId++)
        {
            limpiarCelda(filaId, colId);
        }
    }
    qDebug() << "Borrar Sudoku";
}

bool ScalianSudoku::esSeguro(size_t fila, size_t col, uint valor)
{
    for (size_t i = 0; i < 9; i++)
    {
        auto celdaFila = obtenerCelda(fila, i).value();
        auto celdaColumna = obtenerCelda(i, col).value();
        if (celdaFila->text().toUInt() == valor || celdaColumna->text().toUInt() == valor)
            return false;
    }
    size_t cuadranteInicioFila = fila - fila % 3;
    size_t cuadranteInicioColumna = col - col % 3;
    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            auto celdaSubcuadrante = obtenerCelda(i + cuadranteInicioFila, j + cuadranteInicioColumna).value();
            if (celdaSubcuadrante->text().toUInt() == valor)
                return false;
        }
    }
    return true;
}

bool ScalianSudoku::resolverSudoku()
{
    for (size_t fila = 0; fila < 9; fila++)
    {
        for (size_t col = 0; col < 9; col++)
        {
            auto celdaValue = obtenerCelda(fila, col).value();
            auto num = celdaValue->text().toUInt();
            if (num == 0)
            {
                for (uint candidato = 1; candidato <= 9; candidato++)
                {
                    if (esSeguro(fila, col, candidato))
                    {
                        escribirCelda(candidato, fila, col, QColor(Qt::gray));
                        if (resolverSudoku())
                            return true;
                        limpiarCelda(fila, col);
                    }
                }
                return false;
            }
        }
    }
    return true;
}

bool ScalianSudoku::chequearColumna(uint fila, uint col, uint valor)
{
    for (uint colCompr = 0; colCompr < 9; ++colCompr)
    {
        if (colCompr != col)
        {
            auto celdaCompr = obtenerCelda(fila, colCompr);
            if (celdaCompr.value()->text().toUInt() == valor)
            {
                escribirCelda(valor, fila, colCompr, QColor(Qt::GlobalColor::red));
                escribirCelda(valor, fila, col, QColor(Qt::GlobalColor::red));
                return false;
            }
        }
    }
    return true;
}

bool ScalianSudoku::chequearFila(uint fila, uint col, uint valor)
{
    for (uint filaCompr = 0; filaCompr < 9; ++filaCompr)
    {
        if (filaCompr != fila)
        {
            auto celdaCompr = obtenerCelda(filaCompr, col);
            if (celdaCompr.has_value() && celdaCompr.value()->text().toUInt() == valor)
            {
                escribirCelda(valor, filaCompr, col, QColor(Qt::GlobalColor::red));
                escribirCelda(valor, fila, col, QColor(Qt::GlobalColor::red));
                return false;
            }
        }
    }
    return true;
}

bool ScalianSudoku::chequearCuadro(uint fila, uint col, uint valor)
{
    int cuadranteInicioFila = fila - fila % 3;
    int cuadranteInicioColumna = col - col % 3;
    for (uint i = 0; i < 3; i++)
    {
        for (uint j = 0; j < 3; j++)
        {
            uint filaCompr = i + cuadranteInicioFila;
            uint colCompr = j + cuadranteInicioColumna;
            if (obtenerCelda(filaCompr, colCompr).value()->text().toUInt() == valor && fila != filaCompr && colCompr != col)
            {
                escribirCelda(valor, filaCompr, colCompr, QColor(Qt::GlobalColor::red));
                escribirCelda(valor, fila, col, QColor(Qt::GlobalColor::red));
                return false;
            }
        }
    }
    return true;
}

bool ScalianSudoku::chequearSudoku()
{
    bool ctrl = true;
    for (uint fila = 0; fila < 9; ++fila)
    {
          for (uint col = 0; col < 9; ++col)
          {
              auto celda = obtenerCelda(fila, col);
              if (celda.has_value())
              {
                  uint valor = celda.value()->text().toUInt();
                  if (valor != 0)
                  {
                      if (chequearColumna(fila, col, valor))
                          if (chequearFila(fila, col, valor))
                              if(chequearCuadro(fila, col, valor))
                              {
                                  escribirCelda(valor, fila, col, QColor(Qt::GlobalColor::black));
                                  continue ;
                              }
                      ctrl = false;
                  }
              }
          }
    }
    qDebug() << "Chequear Sudoku";
    return ctrl;
}

void ScalianSudoku::setearCelda(uint filaId, uint colId, uint valor)
{
    if (escribirCelda(valor, filaId, colId, QColor(Qt::GlobalColor::gray)))
        qDebug() << "Setear Celda (" << filaId << "," << colId << "): " << valor;
    else
        qDebug() << "Error: No se pudo obtener la celda en (" << filaId << "," << colId << ").";
}

void ScalianSudoku::borrarCelda(uint filaId, uint colId)
{
    if (limpiarCelda(filaId,colId))
        qDebug() << "Borrar Celda ("<< filaId << "," << colId << ")";
    else
        qDebug() << "Error: No se pudo obtener la celda en (" << filaId << "," << colId << ").";
}

ScalianSudoku::~ScalianSudoku()
{
    delete ui;
}

void ScalianSudoku::onDobleClickEnCelda(uint filaId, uint colId)
{
    ui->FrameCeldas->setProperty("fila", filaId);
    ui->FrameCeldas->setProperty("col", colId);

    ui->EtiquetaFila->setText(QString::number(filaId));
    ui->EtiquetaColumna->setText(QString::number(colId));
    ui->FrameCeldas->setVisible(true);
    ui->FrameControles->setVisible(false);
}

std::optional<QLabel*> ScalianSudoku::obtenerCelda(uint filaId, uint colId)
{
    if(filaId > 9 || colId > 9)
    {
        return std::nullopt;
    }

    if(colId > 5)
    {
        colId += 2;
    }
    else if(colId > 2)
    {
        colId += 1;
    }

    if(filaId > 5)
    {
        filaId += 2;
    }
    else if(filaId > 2)
    {
        filaId += 1;
    }

    auto fila = ui->Tablero->itemAt(filaId)->layout();
    auto celda = dynamic_cast<QLabel*>(fila->itemAt(colId)->widget());

    if(not celda)
    {
        return std::nullopt;
    }

    return celda;
}

bool ScalianSudoku::limpiarCelda(uint filaId, uint colId)
{
    auto celda = obtenerCelda(filaId, colId);
    if(celda.has_value())
    {
        QColor color(Qt::GlobalColor::black);
        celda.value()->setStyleSheet(QString("QLabel { color : rgb(%1,%2,%3); }").arg(color.red()).arg(color.green()).arg(color.blue()));
        celda.value()->setText("");
        return true;
    }

    return false;
}

bool ScalianSudoku::escribirCelda(uint valor, uint filaId, uint colId, QColor color)
{
    auto celda = obtenerCelda(filaId, colId);
    if(celda.has_value() && valor < 10)
    {
        celda.value()->setStyleSheet(QString("QLabel { color : rgb(%1,%2,%3); }").arg(color.red()).arg(color.green()).arg(color.blue()));
        celda.value()->setText(QString::number(valor));
        return true;
    }

    return false;
}

void ScalianSudoku::escribirResultado(const std::string &resultado, QColor color)
{
    ui->EtiquetaResultado->setStyleSheet(QString("QLabel { color : rgb(%1,%2,%3); }").arg(color.red()).arg(color.green()).arg(color.blue()));
    ui->EtiquetaResultado->setText(resultado.c_str());
}

void ScalianSudoku::onLimpiarSudoku()
{
    escribirResultado("");
    limpiarSudoku();
}

int ScalianSudoku::contarElementos()
{
    int cont;

    cont = 0;
    for (int fila = 0; fila < 9; fila++)
    {
        for (int col = 0; col < 9; col++)
        {
            auto celda = obtenerCelda(fila, col);
            int valor = celda.value()->text().toUInt();
            if (valor != 0)
                cont ++;
        }
    }
    return cont;
}

void ScalianSudoku::onResolverSudoku()
{
    bool resultado;
    if (contarElementos() < 17)
    {
        escribirResultado("Incorrecto", QColor(Qt::GlobalColor::yellow));
        return;
    }
    resultado = chequearSudoku();
    if(resultado)
    {
        if (resolverSudoku())
            escribirResultado("Correcto", QColor(Qt::GlobalColor::green));
        else
            escribirResultado("Imposible", QColor(Qt::GlobalColor::red));
    }
    else
        escribirResultado("Imposible", QColor(Qt::GlobalColor::red));
}

void ScalianSudoku::onAceptar()
{
    uint fila = ui->FrameCeldas->property("fila").value<uint>();
    uint col = ui->FrameCeldas->property("col").value<uint>();
    uint valor = ui->ValorCelda->value();
    ui->FrameCeldas->setVisible(false);
    ui->FrameControles->setVisible(true);
    setearCelda(fila, col, valor);
}

void ScalianSudoku::onCancelar()
{
    ui->FrameCeldas->setVisible(false);
    ui->FrameControles->setVisible(true);
}

void ScalianSudoku::onBorrar()
{
    uint fila = ui->FrameCeldas->property("fila").value<uint>();
    uint col = ui->FrameCeldas->property("col").value<uint>();
    ui->FrameCeldas->setVisible(false);
    ui->FrameControles->setVisible(true);
    limpiarCelda(fila, col);
    borrarCelda(fila, col);
}

bool ScalianSudoku::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonDblClick)
    {
        if(sudokuVacio)
        {
            auto coordinates = obtenerCoordenadas(object);
            if(coordinates.has_value())
            {
                uint fila = std::get<0>(coordinates.value());
                uint col = std::get<1>(coordinates.value());
                onDobleClickEnCelda(fila, col);
            }
        }
        else
        {
            // Popup
        }
    }

    return QMainWindow::eventFilter(object, event);
}

std::optional<std::tuple<uint, uint>> ScalianSudoku::obtenerCoordenadas(QObject *object)
{
    if(object)
    {
        auto label = dynamic_cast<QLabel*>(object);
        if(label)
        {
            uint fila = object->property("fila").value<uint>();
            uint col = object->property("col").value<uint>();
            return std::tuple<int,int>{fila,col};
        }
    }

    return std::nullopt;
}

