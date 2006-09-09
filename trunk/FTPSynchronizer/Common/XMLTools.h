#include <QDomElement>

class cXMLTools
{
	public:
		static void SetText(QDomDocument qddDocument,
								  QDomElement *qdeElement,
								  const QString qsText);
}; // cXMLTools