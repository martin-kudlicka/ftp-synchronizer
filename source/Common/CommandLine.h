#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include <QStringList>
#include <QHash>

class cCommandLine {
	public:
		QStringList qslArguments;

		void AddKeyValue(const QString qsOption, QString *qsValue);
		void Parse();

	private:
		QHash<QString, QString *> qhKeyValues;

		bool IsKeyValue(const QString qsKey);
		void SetKeyValue(const QString qsKey, const QString qsValue);
}; // cCommandLine

#endif
