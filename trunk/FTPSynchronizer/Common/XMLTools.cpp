#include "XMLTools.h"

// sets inner text of QDomElement
void cXMLTools::SetText(QDomDocument qddDocument,
								QDomElement *qdeElement,
								const QString qsText)
{
	QDomText qdtText;

	if (qdeElement->text() != "") {
		// remove old text
		qdtText = qdeElement->firstChild().toText();
		qdtText.setData(qsText);
	} else {
		qdtText = qddDocument.createTextNode(qsText);
	} // if else
	qdeElement->appendChild(qdtText);
} // SetText