#pragma once
#ifndef _GRAMMAR_ANALYSIS_
#define _GRAMMAR_ANALYSIS_

#include "LexicalAnalysis.h"

#include "GrammarErrorType.h"

#include "TokenAnalysis.h"

#include "KH_TextList.h"

#include <string.h>
#include <vector>



namespace KH {

	class GrammarPL0{
	public :
		typedef KH::LA_Def			LA_Def;
		typedef KH::_ErrorType		ErrorType;
		typedef KH::_ErrorTypeList	ErrorTypeList;
		typedef KH::Token			Token;
		typedef std::vector<Token>		TokenVector ;
		typedef std::vector<TokenVector>	TokenMap	;
		typedef std::vector<std::string>	ErrorListOutputType ;
		typedef KH::LexicalAnalysis LA;

		//ʹ��LA��ʼ��
		GrammarPL0(LA &la){
			pla = &la;
			tkMap.clear();
			Reset();
		}

		//ʹ��LA��TextList��ʼ��
		GrammarPL0(LA &la,KH::TextList tl){
			pla = &la;
			tkMap.clear();
			GetInput(tl);
			Reset();	
		}

		//����һ��textlist��ת��Ϊtokenlist
		void GetInput(KH::TextList tl){
			Reset();
			KH::TextList::iterator it = tl.begin();
			
			TokenVector TV;
			std::string str;
			tkMap.clear();
			for(int i = 0;it != tl.end(); it++,i++)	{
				str = tl.GetString(it);
				TV = GetTokenVector(i,str);
				if(TV.size() >0)
				tkMap.push_back(TV);
			}
			Reset(false);
		}

		//��һ��string���뵽һ��Token����
		TokenVector GetTokenVector(TokenVector TV, int line, std::string str){
			Token tk;
			int word = 0;
			bool blank = true;
			while(!str.empty()) {
				tk = pla->GetAToken(str);
				if(!tk.IsDefined()) {
					char sword[20] = {0};
					sprintf(sword,"%d",word);
					ErrorList.push_back(ErrorType(line," " + std::string(sword) + " " + tk.GetValue() +  " | Undefine Word"));//
				}
				else {
					if(tk.GetSign() != KH::_BLOCK){
						word += 1;
						TV.push_back(tk);
						blank = false;
					}
				}
			}
			if(blank)TV.push_back(Token("",KH::_BLOCK));
			return TV;
		}

		TokenVector GetTokenVector(int line, std::string str){
			TokenVector TV = GetTokenVector(TV,line,str);
			return TV;
		}
		
		//��ȡ��ǰָ����ָλ�õ�Token
		Token GetWord(bool AddToNext = false){
			Token word = Token();
			if(itLine == tkMap.end())return word;
			if(itWord != itLine->end())word = *itWord;
			if(AddToNext)
			{
				if(itWord != itLine->end()){
					itWord ++;
				}
				if(itWord == itLine->end()){
					itLine ++;
					itWord = itLine->begin();
				}
			}
			else return word;
		}

		//��ȡ��ǰָ����ָ+ƫ����λ�õ�Token
		Token GetWord(int offset, bool NotSkipLine) const{
			TokenMap::iterator itL = itLine;
			TokenVector::iterator itW = itWord;

			if( itL == tkMap.end() ) return Token();

			for(int i = 0 ; i < offset ; i ++ ) {
				itW ++;
				if(itW == itL->end()){
					if( NotSkipLine == true)return Token();
					itL++;
					if(itL != tkMap.end())
						itW == itL->begin();
					else return Token();
				}
				if(itL != tkMap.end() && itW != itLine->end())
					if(itW->GetSign() == KH::_BLOCK) 
						offset += 1;
			}

			return *itW;
		}

		//����Ƿ�ĩβ
		bool CheckEnd() {
			if(itLine == tkMap.end() || nLine >= tkMap.size())
				return true;
			return false;
		}
		
		bool ExertGrammar(bool isGen = false);

		//��ǰָ����ǰ��
		GrammarPL0& skip(int offset, bool NotSkipLine = false){
			if(itLine == tkMap.end())
					return *this;
			for(int i = 0 ; i < offset ; i ++ ) {
				if(itWord != itLine->end())
					itWord ++;
				if(itWord == itLine->end()){
					if( NotSkipLine == true){
						break;
					}
					LineMoveForward();
				}
				if(itLine != tkMap.end() && itWord != itLine->end())
					if(itWord->GetSign() == KH::_BLOCK) 
						offset += 1;
			}
			return *this;
		}

		//����һ��
		void LineMoveForward(){	//TODO����ʱ��Խ�磡��������khtest�Ĵ�����end֮ǰ�Ӹ�a
			if(itLine != tkMap.end()){
				nLine++;
				itLine++;
				if(itLine != tkMap.end())
					itWord = itLine->begin();
			}
		}

		//��õ�ǰ��ָ��
		int GetLine(){
			return nLine;
		}

		//���������Ϣ
		void InsertError(ErrorType e){
			ErrorList.push_back(e);
		}

		//���������Ϣ�ı�
		ErrorListOutputType OutErrorList(){ 
			char str[20]="";
			ErrorListOutputType sL;
			ErrorTypeList::iterator it = ErrorList.begin() ;
			for(;it != ErrorList.end(); it++){
				sprintf(str,"%d",it->GetLine());
				sL.push_back( std::string(str) + it->Message());
			}
			return sL;
		}

		//�Ƿ�ɹ�
		bool IsPassed(){
			return ErrorList.empty();
		}

		//���������Ϣ��string��
		std::string	OutErrorString(){
			ErrorListOutputType sL = OutErrorList();
			ErrorListOutputType::iterator it = sL.begin();
			std::string str = "";
			for(;it != sL.end(); it++){
				str += (*it + "\n");
			}
			if(str.length() <= 1)str = "Success\n";
			return str;
		}

		//���õ�ǰλ�úʹ�����Ϣ��
		void Reset(bool ClearErrorList = true){
			nLine = 0;
			if(ClearErrorList)
				ErrorList.clear();
			itLine = tkMap.begin();
			if(itLine != tkMap.end())
				itWord = itLine->begin();
		}

//#======================================================================================

	private :

		bool GrammarSign(KH::GrammarPL0::Token t,const int d);
		//������
		void Error(bool &result, std::string message, bool skip = true );
		//�ֳ����﷨�ж�
		bool Block();
		//����
		bool ConditionParsing();
		//"���ʽ"�﷨����
		bool ExpressionParsing();
		//"��"�﷨����
		bool TermParsing();
		//"����"�﷨����
		bool FactorParsing();
		//"���"�﷨����
		bool StatementParsing();
//#======================================================================================
		void GenPush(std::string str){ //���ɵ�C����
			if(IsGenerate && !str.empty())
				sGens.push_back(str);
		}

	private	:
		TokenMap tkMap;
		int nLine;
		TokenMap::iterator itLine;
		//TokenVector	tkVector;
		TokenVector::iterator itWord;
		LA* pla;
		ErrorTypeList ErrorList;
		
		bool IsGenerate;
	public:
		std::vector<std::string> sGens;
		std::vector<std::string> funcNames;
	};//end of class GrammarPL0

}; // end of namespace 


#endif