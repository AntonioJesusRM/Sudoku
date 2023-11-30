#ifndef SCALIANSUDOKU_H
#define SCALIANSUDOKU_H

#include <QMainWindow>

#include <optional>
#include <tuple>
#include <set>
#include <QLabel>
#include <QColor>

QT_BEGIN_NAMESPACE
namespace Ui { class ScalianSudoku; }
QT_END_NAMESPACE

class ScalianSudoku : public QMainWindow
{
    Q_OBJECT

public:
    ScalianSudoku(QWidget *parent = nullptr);

    virtual void limpiarSudoku();
    virtual bool resolverSudoku();
    virtual bool chequearSudoku();
    virtual bool esSeguro(size_t fila, size_t col, uint valor);
    virtual bool chequearFila(uint fila, uint col, uint valor);
    virtual bool chequearColumna(uint fila, uint col, uint valor);
    virtual bool chequearCuadro(uint fila, uint col, uint valor);
    virtual void setearCelda(uint filaId, uint colId, uint valor);
    virtual void borrarCelda(uint filaId, uint colId);

    ~ScalianSudoku();

protected:

    virtual void onDobleClickEnCelda(uint filaId, uint colId);
    std::optional<QLabel*> obtenerCelda(uint filaId, uint colId);

    bool limpiarCelda(uint filaId, uint colId);
    bool escribirCelda(uint valor, uint filaId, uint colId, QColor color = QColor(Qt::GlobalColor::black));
    void escribirResultado(const std::string &resultado, QColor color = QColor(Qt::GlobalColor::black));
    int contarElementos();
    void rellenarSudoku();

    void onLimpiarSudoku();
    void onResolverSudoku();
    void onAceptar();
    void onCancelar();
    void onBorrar();

private:    

    bool eventFilter(QObject *object, QEvent *event) override;
    std::optional<std::tuple<uint, uint>> obtenerCoordenadas(QObject *object);

    Ui::ScalianSudoku *ui;
    bool sudokuVacio;
};
#endif // SCALIANSUDOKU_H
