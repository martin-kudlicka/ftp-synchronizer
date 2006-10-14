#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include <QString>
#include <QHash>

class cCommandLine {
	public:
		char **cArguments;
		int iArguments;

		void AddKeyValue(const QString qsOption, QString *qsValue);
		void Parse();

	private:
		QHash<QString, QString *> qhKeyValues;

		bool IsKeyValue(const QString qsKey);
		void SetKeyValue(const QString qsKey, const QString qsValue);
}; // cCommandLine

#endif