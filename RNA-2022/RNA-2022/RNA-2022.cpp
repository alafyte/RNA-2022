#include "stdafx.h"
#include "Error.h"
#include "Parm.h"
#include "Log.h"
#include "In.h"
#include "LT.h"
#include "LexAnalysis.h"
#include "Polish.h"
#include "MFST.h"
#include "Greibach.h"
#include "SemAnalysis.h"
#include "Generation.h"


int wmain(int argc, wchar_t* argv[])
{
	setlocale(LC_ALL, "ru");
	Log::LOG logfile = Log::INITLOG;

	try
	{
		Parm::PARM parm = Parm::getparm(argc, argv);
		logfile = Log::getlog(parm.log);
		In::IN in = In::getin(parm);
		Log::WriteLog(logfile);
		Log::WriteParm(logfile, parm);
		Log::WriteIn(logfile, in);

		LexAnalisys::LA lex = LexAnalisys::Analyze(in);
		Semantics::AnalyzeSem(lex);

		Log::WriteIDTableLog(lex.idtable, logfile);
		Log::WriteLexTableLog(lex.lextable, logfile);
		Log::WritePolizLog(lex.lextable, logfile);

		MFST_TRACE_START(logfile);
		unsigned int start_time = clock();
		MFST::Mfst mfst(lex.lextable, GRB::getGreibach());
		bool syntaxAnalysisRes = mfst.start(logfile);
		unsigned int end_time = clock();
		unsigned int search_time = end_time - start_time;
		mfst.savededucation();
		mfst.printrules(logfile);

		*logfile.stream << "\tÐàçáîð âûïîëíåí çà " << ((float)search_time) / CLOCKS_PER_SEC << " ñåêóíä." << endl;
		*logfile.stream << setfill('=') << setw(90) << '=' << endl;

		cout << "\tÐàçáîð âûïîëíåí çà " << ((float)search_time) / CLOCKS_PER_SEC << " ñåêóíä." << endl;
		cout << setfill('=') << setw(100) << '=' << endl;


		if (parm.idtable)
			IT::WriteTable(lex.idtable);
		if (parm.lextable)
			LT::WriteLexTable(lex.lextable);

		if (syntaxAnalysisRes && Poliz::StartPoliz(lex))
		{

			cout << "\t\tÏÎËÜÑÊÀß ÍÎÒÀÖÈß ÂÛÏÎËÍÅÍÀ" << endl;
			cout << setfill('=') << setw(100) << '=' << endl;
			if (parm.poliz)
				LT::WritePoliz(lex.lextable);

			*logfile.stream << "\t\tÏÎËÜÑÊÀß ÍÎÒÀÖÈß ÂÛÏÎËÍÅÍÀ" << endl;
			*logfile.stream << setfill('=') << setw(90) << '=' << endl;
			Log::WritePolizLog(lex.lextable, logfile);

			GenerateAsm::Generation(lex.lextable, lex.idtable, parm.out);
		}
		else
		{
			cout << "\t\tÏÎËÜÑÊÀß ÍÎÒÀÖÈß ÍÅ ÂÛÏÎËÍÅÍÀ. ÏÐÎÈÇÎØËÀ ÎØÈÁÊÀ" << endl;
			cout << setfill('=') << setw(100) << '=' << endl;
			*logfile.stream << "\t\tÏÎËÜÑÊÀß ÍÎÒÀÖÈß ÍÅ ÂÛÏÎËÍÅÍÀ. ÏÐÎÈÇÎØËÀ ÎØÈÁÊÀ" << endl;
			*logfile.stream << setfill('=') << setw(90) << '=' << endl;
		}

		IT::Delete(lex.idtable);
		LT::Delete(lex.lextable);
	}
	catch (Error::ERROR e)
	{
		Log::WriteError(logfile, e);
		cout << "Îøèáêà " << e.id << ": " << e.message << endl;
	}

	return 0;
}