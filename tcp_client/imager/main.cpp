//------------------------------------------------------------------------------

#include <QApplication>
#include <QCommandLineParser>

//------------------------------------------------------------------------------

//#include "imageviewer.hpp"
#include "./imager_lib/Resize_image_dialog.hpp"
#include "./imager_lib/Transmit_image_dialog.hpp"

//------------------------------------------------------------------------------

using namespace Imager;

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    /*QGuiApplication::setApplicationDisplayName(ImageViewer::tr("Image Viewer"));
    QCommandLineParser commandLineParser;
    commandLineParser.addHelpOption();
    commandLineParser.addPositionalArgument(ImageViewer::tr("[file]"), ImageViewer::tr("Image file to open."));
    commandLineParser.process(QCoreApplication::arguments());*/
    QImage img;
    QString str;
    Transmit_image_dialog imageViewer(img, str);
    //imageViewer.set_original_size(QSize(123,233));
    /*if (!commandLineParser.positionalArguments().isEmpty()
        && !imageViewer.loadFile(commandLineParser.positionalArguments().front())) {
        return -1;
    }*/
    imageViewer.show();
    return app.exec();
}

//------------------------------------------------------------------------------
