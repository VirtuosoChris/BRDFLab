#include "PropertyEditorWidget.h"

using namespace std;

PropertyEditorWidget::PropertyEditorWidget(){

	comboBox = new QComboBox(this);

	brdfOptions = new QPropertyEditorWidget(this);

	button = new QPushButton("Add Lobe", this);

	//QPushButton *optimizeButton = new QPushButton("Optimize Parameters", this);

	connect(button, SIGNAL(clicked()), this, SLOT(addLobe()));
	  

	QLabel *channelLabel = new QLabel("Channel displayed:");
	channelLabel->setAlignment(Qt::AlignRight);

	QComboBox *channelDisplayed = new QComboBox();
	channelDisplayed->addItem("0");
	channelDisplayed->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	QHBoxLayout *channelSet = new QHBoxLayout();
	channelSet->addWidget(channelLabel);
	channelSet->addWidget(channelDisplayed);
	


	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(comboBox);
	layout->addWidget(button);
	layout->addWidget(brdfOptions);
	//layout->addLayout(channelSet);
	//layout->addWidget(optimizeButton);


	setLayout(layout);

 
//	AddLobe *al = new AddLobe();
//	brdfOptions->addObject(al);
/*
	ScalingFactorsParameters3 *sc = new ScalingFactorsParameters3(brdfOptions);
	brdfOptions->addObject(sc);

	LafortuneIsoParameters *lip = new LafortuneIsoParameters(brdfOptions);
	brdfOptions->addObject(lip);

*/

}

void
PropertyEditorWidget::addLobe(){

	QStringList items;
    items << tr("Lambertian") << tr("BlinnPhong")<< tr("LafortuneIso")<< tr("LafortuneAniso")<< tr("AshikhminShirley");

	bool ok;
	QString item = QInputDialog::getItem(this, tr("BRDFLab: New Lobe"),
                                          tr("Model:"), items, 0, false, &ok);

	if (!ok)
		return;

	brdf->addLobe(item);

}