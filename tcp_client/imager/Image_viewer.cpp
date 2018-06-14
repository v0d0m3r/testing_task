//------------------------------------------------------------------------------

#include <QtWidgets>
#include <QAction>

//------------------------------------------------------------------------------

#include "Image_viewer.hpp"
#include "../imager_lib/Transmit_image_dialog.hpp"

//------------------------------------------------------------------------------

namespace Imager {

//------------------------------------------------------------------------------

Image_viewer::Image_viewer()
    : image_label(new QLabel),
      scroll_area(new QScrollArea),
      scale_factor(1)
{
    image_label->setBackgroundRole(QPalette::Base);
    image_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    image_label->setScaledContents(true);

    scroll_area->setBackgroundRole(QPalette::Dark);
    scroll_area->setWidget(image_label);
    scroll_area->setVisible(false);
    setCentralWidget(scroll_area);

    create_actions();

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}

//------------------------------------------------------------------------------

bool Image_viewer::load_file(const QString& file_name)
{
    QImageReader reader(file_name);
    reader.setAutoTransform(true);
    const QImage new_image = reader.read();
    if (new_image.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(file_name), reader.errorString()));
        return false;
    }

    curr_img_feat = Image_features(new_image.size(),reader.quality());
    prev_img_feat = curr_img_feat;
    curr_name_image = file_name;

    set_image(new_image);

    setWindowFilePath(file_name);

    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
        .arg(QDir::toNativeSeparators(file_name)).arg(image.width()).arg(image.height()).arg(image.depth());
    statusBar()->showMessage(message);
    return true;
}

//------------------------------------------------------------------------------

void Image_viewer::set_image(const QImage& new_image)
{
    image = new_image;
    image_label->setPixmap(QPixmap::fromImage(image));
    scale_factor = 1.0;

    scroll_area->setVisible(true);
    fit_to_window_act->setEnabled(true);
    update_actions();

    if (!fit_to_window_act->isChecked())
        image_label->adjustSize();
}

//------------------------------------------------------------------------------

bool Image_viewer::save_file(const QString& file_name)
{
    QImageWriter writer(file_name);
    writer.setCompression(curr_img_feat.info.compression_level);
    writer.setQuality(curr_img_feat.info.quality);

    qDebug() << writer.compression();
    qDebug() << writer.quality();

    if (!writer.write(image)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot write %1: %2")
                                 .arg(QDir::toNativeSeparators(file_name)), writer.errorString());
        return false;
    }
    const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(file_name));
    statusBar()->showMessage(message);

    return true;
}

//------------------------------------------------------------------------------

static void init_image_file_dialog(QFileDialog& dialog,
                                   QFileDialog::AcceptMode accept_mode)
{
    static bool first_dialog = true;

    if (first_dialog) {
        first_dialog = false;
        const QStringList pictures_locations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(pictures_locations.isEmpty()
                            ? QDir::currentPath() : pictures_locations.last());
    }

    QStringList mime_type_filters;
    const auto supported_mime_types = (accept_mode == QFileDialog::AcceptOpen)
            ? QImageReader::supportedMimeTypes()
            : QImageWriter::supportedMimeTypes();

    foreach (const auto& mime_type_name, supported_mime_types)
        mime_type_filters.append(mime_type_name);

    mime_type_filters.sort();
    dialog.setMimeTypeFilters(mime_type_filters);
    dialog.selectMimeTypeFilter("image/png");
    if (accept_mode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("png");
}

//------------------------------------------------------------------------------

void Image_viewer::open_cb()
{
    QFileDialog dialog(this, tr("Open File"));
    init_image_file_dialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !load_file(dialog.selectedFiles().first())) {}
}

//------------------------------------------------------------------------------

void Image_viewer::save_cb()
{
    prev_img_feat = curr_img_feat;
    previous_state_act->setEnabled(false);

    save_file(curr_name_image);
}

//------------------------------------------------------------------------------

void Image_viewer::save_as_cb()
{
    QFileDialog dialog(this, tr("Save File As"));
    init_image_file_dialog(dialog, QFileDialog::AcceptSave);

    while (dialog.exec() == QDialog::Accepted && !save_file(dialog.selectedFiles().first())) {}
}

//------------------------------------------------------------------------------

void Image_viewer::transmission_cb()
{
    Q_ASSERT(image_label->pixmap());
    Transmit_image_dialog d(image);
    d.exec();
}

//------------------------------------------------------------------------------

void Image_viewer::copy_cb()
{
#ifndef QT_NO_CLIPBOARD
    QGuiApplication::clipboard()->setImage(image);
#endif // !QT_NO_CLIPBOARD
}

//------------------------------------------------------------------------------

#ifndef QT_NO_CLIPBOARD
static QImage clipboardImage()
{
    if (const QMimeData* mime_data = QGuiApplication::clipboard()->mimeData()) {
        if (mime_data->hasImage()) {
            const QImage image = qvariant_cast<QImage>(mime_data->imageData());
            if (!image.isNull())
                return image;
        }
    }
    return QImage();
}
#endif // !QT_NO_CLIPBOARD

//------------------------------------------------------------------------------

void Image_viewer::paste_cb()
{
#ifndef QT_NO_CLIPBOARD
    const QImage new_image = clipboardImage();
    if (new_image.isNull())
        statusBar()->showMessage(tr("No image in clipboard"));
    else {
        curr_img_feat = Image_features(new_image.size());
        prev_img_feat = curr_img_feat;

        set_image(new_image);
        setWindowFilePath(QString());
        const QString message = tr("Obtained image from clipboard, %1x%2, Depth: %3")
            .arg(new_image.width()).arg(new_image.height()).arg(new_image.depth());
        statusBar()->showMessage(message);
    }
#endif // !QT_NO_CLIPBOARD
}

//------------------------------------------------------------------------------

void Image_viewer::resize_cb()
{
    Q_ASSERT(image_label->pixmap());
    Resize_image_dialog rid;
    rid.set_original_size(curr_img_feat.size);

    if (rid.exec() == QDialog::Accepted) {
        prev_img_feat.size = curr_img_feat.size;
        curr_img_feat.size = rid.size();

        if (prev_img_feat.size == curr_img_feat.size) {
            previous_state_act->setEnabled(curr_img_feat.info != prev_img_feat.info);
            return;
        }

        set_image(image.scaled(curr_img_feat.size,
                              Qt::IgnoreAspectRatio,
                              Qt::SmoothTransformation));
        previous_state_act->setEnabled(true);
        save_act->setEnabled(true);
    }
}

//------------------------------------------------------------------------------

void Image_viewer::compress_cb()
{
    Q_ASSERT(image_label->pixmap());
    Compress_image_dialog cid;
    cid.set_original_info(curr_img_feat.info);
    if (cid.exec() == QDialog::Accepted) {
        prev_img_feat.info = curr_img_feat.info;
        curr_img_feat.info = cid.compression_info();

        if (curr_img_feat.info == prev_img_feat.info)
            previous_state_act->setEnabled(prev_img_feat.size != curr_img_feat.size);
        else {
            previous_state_act->setEnabled(true);
            save_act->setEnabled(true);
        }
    }
}

//------------------------------------------------------------------------------

void Image_viewer::previous_cb()
{
    Q_ASSERT(image_label->pixmap());
    bool enabled = true;
    if (prev_img_feat.size != curr_img_feat.size) {
        set_image(image.scaled(prev_img_feat.size));
        curr_img_feat.size = prev_img_feat.size;

        enabled = false;
    }
    if (       prev_img_feat.info.compression_level != curr_img_feat.info.compression_level
               || prev_img_feat.info.quality != curr_img_feat.info.quality) {
        curr_img_feat.info = prev_img_feat.info;
        enabled = false;
    }
    previous_state_act->setEnabled(enabled);
}

//------------------------------------------------------------------------------

void Image_viewer::zoom_in_cb()
{
    scale_image(1.25);
}

//------------------------------------------------------------------------------

void Image_viewer::zoom_out_cb()
{
    scale_image(0.8);
}

//------------------------------------------------------------------------------

void Image_viewer::normal_size_cb()
{
    image_label->adjustSize();
    scale_factor = 1.0;
}

//------------------------------------------------------------------------------

void Image_viewer::fit_to_window_cb()
{
    bool fit_to_window = fit_to_window_act->isChecked();
    scroll_area->setWidgetResizable(fit_to_window);
    if (!fit_to_window)
        normal_size_cb();
    update_actions();
}

//------------------------------------------------------------------------------

void Image_viewer::about_cb()
{
    QMessageBox::about(this, tr("About Image Viewer"),
            tr("<p>The <b>Image Viewer</b> is first aplication of testing task."
               "For more information follow to "
               "https://github.com/v0d0m3r/testing_task</p>"));
}

//------------------------------------------------------------------------------

void Image_viewer::create_actions()
{
    auto file_menu = menuBar()->addMenu(tr("&File"));

    auto open_act = file_menu->addAction(tr("&Open..."), this, &Image_viewer::open_cb);
    open_act->setShortcut(QKeySequence::Open);

    save_act = file_menu->addAction(tr("&Save"), this, &Image_viewer::save_cb);
    save_act->setShortcut(Qt::CTRL + Qt::Key_S);
    save_act->setEnabled(false);

    save_as_act = file_menu->addAction(tr("Save &As..."), this, &Image_viewer::save_as_cb);
    save_as_act->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_S);
    save_as_act->setEnabled(false);

    file_menu->addSeparator();

    transmission_act = file_menu->addAction(tr("&Transmission"),
                                           this, &Image_viewer::transmission_cb);
    transmission_act->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_T);
    transmission_act->setEnabled(false);

    file_menu->addSeparator();

    auto exit_act = file_menu->addAction(tr("E&xit"), this, &QWidget::close);
    exit_act->setShortcut(tr("Ctrl+Q"));

    auto edit_menu = menuBar()->addMenu(tr("&Edit"));

    copy_act = edit_menu->addAction(tr("&Copy"), this, &Image_viewer::copy_cb);
    copy_act->setShortcut(QKeySequence::Copy);
    copy_act->setEnabled(false);

    auto paste_act = edit_menu->addAction(tr("&Paste"), this, &Image_viewer::paste_cb);
    paste_act->setShortcut(QKeySequence::Paste);

    edit_menu->addSeparator();

    resize_act = edit_menu->addAction(tr("&Resize"), this, &Image_viewer::resize_cb);
    resize_act->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_R));
    resize_act->setEnabled(false);

    compress_act = edit_menu->addAction(tr("Co&mpress"), this, &Image_viewer::compress_cb);
    compress_act->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_M));
    compress_act->setEnabled(false);

    previous_state_act = edit_menu->addAction(tr("Pr&evious"), this, &Image_viewer::previous_cb);
    previous_state_act->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_E));
    previous_state_act->setEnabled(false);

    auto view_menu = menuBar()->addMenu(tr("&View"));

    zoom_in_act = view_menu->addAction(tr("Zoom &In (25%)"), this, &Image_viewer::zoom_in_cb);
    zoom_in_act->setShortcut(QKeySequence::ZoomIn);
    zoom_in_act->setEnabled(false);

    zoom_out_act = view_menu->addAction(tr("Zoom &Out (25%)"), this, &Image_viewer::zoom_out_cb);
    zoom_out_act->setShortcut(QKeySequence::ZoomOut);
    zoom_out_act->setEnabled(false);

    normal_size_act = view_menu->addAction(tr("&Normal Size"), this, &Image_viewer::normal_size_cb);
    normal_size_act->setShortcut(tr("Ctrl+S"));
    normal_size_act->setEnabled(false);

    view_menu->addSeparator();

    fit_to_window_act = view_menu->addAction(tr("&Fit to Window"), this, &Image_viewer::fit_to_window_cb);
    fit_to_window_act->setEnabled(false);
    fit_to_window_act->setCheckable(true);
    fit_to_window_act->setShortcut(tr("Ctrl+F"));

    auto help_menu = menuBar()->addMenu(tr("&Help"));

    help_menu->addAction(tr("&About"), this, &Image_viewer::about_cb);
}

//------------------------------------------------------------------------------

void Image_viewer::update_actions()
{
    save_as_act->setEnabled(!image.isNull());
    transmission_act->setEnabled(!image.isNull());
    copy_act->setEnabled(!image.isNull());
    resize_act->setEnabled(!image.isNull());
    compress_act->setEnabled(!image.isNull());
    zoom_in_act->setEnabled(!fit_to_window_act->isChecked());
    zoom_out_act->setEnabled(!fit_to_window_act->isChecked());
    normal_size_act->setEnabled(!fit_to_window_act->isChecked());
}

//------------------------------------------------------------------------------

void Image_viewer::scale_image(double factor)
{
    Q_ASSERT(image_label->pixmap());
    scale_factor *= factor;
    image_label->resize(scale_factor * image_label->pixmap()->size());

    adjust_scroll_bar(scroll_area->horizontalScrollBar(), factor);
    adjust_scroll_bar(scroll_area->verticalScrollBar(), factor);

    zoom_in_act->setEnabled(scale_factor < 3.0);
    zoom_out_act->setEnabled(scale_factor > 0.333);
}

//------------------------------------------------------------------------------

void Image_viewer::adjust_scroll_bar(QScrollBar* scroll_bar, double factor)
{
    if (scroll_bar == nullptr)
        return;
    scroll_bar->setValue(int(factor * scroll_bar->value()
                             + ((factor - 1) * scroll_bar->pageStep()/2)));
}

//------------------------------------------------------------------------------

}   // Imager
//------------------------------------------------------------------------------
