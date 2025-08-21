#ifndef MYQGRAPHICSVIEW_H
#define MYQGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMenu>
#include <QAction>

class MyQGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    explicit MyQGraphicsView(QWidget *parent = nullptr);
    void setPixmap(const QPixmap &pixmap); // 设置图像
    void setImage(const QImage& image);
    void resetView();                      // 重置视图（居中+恢复默认缩放）

signals:
    void rightClicked(QPoint pos);          // 右键点击信号

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override; // 右键菜单事件

private:
    QGraphicsScene *m_scene;                // 场景对象
    QGraphicsPixmapItem *m_imageItem;        // 图像图元
    QPointF m_dragStartPos;                  // 拖拽起始点
    bool m_isDragging;                       // 拖拽状态标志
    qreal m_zoomFactor;                      // 当前缩放因子
    qreal m_defaultZoom;                     // 默认缩放因子（适应视图）
    QMenu *m_contextMenu;                    // 右键菜单

    void initScene();                        // 初始化场景
    void updateImagePosition();              // 更新图像位置（居中）
};

#endif // MYQGRAPHICSVIEW_H