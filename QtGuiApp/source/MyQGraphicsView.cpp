#include "MyQGraphicsView.h"

#define ZOOM_MAX 5.0    // 最大缩放倍数
#define ZOOM_MIN 0.1    // 最小缩放倍数
#define ZOOM_STEP 0.2   // 单次缩放步长

MyQGraphicsView::MyQGraphicsView(QWidget *parent)
    : QGraphicsView(parent), 
      m_scene(nullptr),
      m_imageItem(nullptr),
      m_isDragging(false),
      m_zoomFactor(1.0),
      m_defaultZoom(1.0) {

    // 初始化场景
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);

    // 优化渲染设置
    setRenderHint(QPainter::Antialiasing, true);      // 抗锯齿
    setRenderHint(QPainter::SmoothPixmapTransform, true); // 平滑图像变换
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate); // 智能视口更新
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 隐藏滚动条
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 初始化右键菜单
    m_contextMenu = new QMenu(this);
    QAction *resetAction = new QAction("重置视图", this);
    connect(resetAction, &QAction::triggered, this, &MyQGraphicsView::resetView);
    m_contextMenu->addAction(resetAction);
}

// 设置图像并自动居中
void MyQGraphicsView::setPixmap(const QPixmap &pixmap) {
    if (m_imageItem) {
        m_scene->removeItem(m_imageItem);
        delete m_imageItem;
    }

    m_imageItem = m_scene->addPixmap(pixmap);
    m_defaultZoom = qMin(width() * 1.0 / pixmap.width(), height() * 1.0 / pixmap.height());
    resetView(); // 调用重置确保居中
}

void MyQGraphicsView::setImage(const QImage& image)
{
    setPixmap(QPixmap::fromImage(image));
}

// 重置视图：居中图像 + 恢复默认缩放
void MyQGraphicsView::resetView() {
    m_zoomFactor = m_defaultZoom;
    if (m_imageItem) {
        m_imageItem->setScale(m_zoomFactor);
        updateImagePosition();
    }
}

// 更新图像位置（居中）
void MyQGraphicsView::updateImagePosition() {
    if (!m_imageItem) return;
    QRectF viewRect = rect();
    QRectF itemRect = m_imageItem->boundingRect();
    qreal x = (viewRect.width() - itemRect.width() * m_zoomFactor) / 2.0;
    qreal y = (viewRect.height() - itemRect.height() * m_zoomFactor) / 2.0;
    m_imageItem->setPos(x, y);
}

// 事件处理函数
void MyQGraphicsView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_dragStartPos = event->pos(); // 记录拖拽起点
        m_isDragging = true;
        setCursor(Qt::ClosedHandCursor); // 手型光标
    }
    QGraphicsView::mousePressEvent(event);
}

void MyQGraphicsView::mouseMoveEvent(QMouseEvent *event) {
    if (m_isDragging) {
        // 计算拖拽偏移量并移动视图
        QPointF delta = mapToScene(event->pos()) - mapToScene(m_dragStartPos.toPoint());
        translate(delta.x(), delta.y());
    }
    QGraphicsView::mouseMoveEvent(event);
}

void MyQGraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        setCursor(Qt::ArrowCursor); // 恢复默认光标
    }
    QGraphicsView::mouseReleaseEvent(event);
}

// 滚轮缩放（以鼠标位置为中心）
void MyQGraphicsView::wheelEvent(QWheelEvent *event) {
    qreal zoomDelta = (event->angleDelta().y() > 0) ? ZOOM_STEP : -ZOOM_STEP;
    qreal newZoom = qBound(ZOOM_MIN, m_zoomFactor + zoomDelta, ZOOM_MAX);

    if (newZoom != m_zoomFactor && m_imageItem) {
        m_zoomFactor = newZoom;
        m_imageItem->setScale(m_zoomFactor);
        
        // 以鼠标位置为缩放中心
        QPointF scenePos = mapToScene(event->position().toPoint());
        QPointF itemPos = m_imageItem->mapFromScene(scenePos);
        QPointF offset = (itemPos * (zoomDelta));
        m_imageItem->moveBy(-offset.x(), -offset.y());
    }
}

// 右键菜单事件
void MyQGraphicsView::contextMenuEvent(QContextMenuEvent *event) {
    m_contextMenu->exec(event->globalPos());
    emit rightClicked(event->pos()); // 发送右键点击信号
}