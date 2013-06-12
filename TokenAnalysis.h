#pragma once

#ifndef _DFA_TOKEN_ANALYSIS
#define _DFA_TOKEN_ANALYSIS

#include<string>

namespace KH{

	template <class _ValueType = std::string, class _SignType = int> 
	class _Token {
	public:
		typedef _SignType SignType;
		typedef _ValueType ValueType;

		//define 
		static const _SignType DefSign = 0; 
		

		//set
		_Token() { sign = DefSign;}
		_Token(_ValueType v, _SignType s) {
			SetToken(v, s);
		}
		void SetToken(_ValueType v, _SignType s) { //�趨token
			sign = s;
			value = v;
		}

		//get
		_ValueType GetValue() const { return value; }
		_SignType GetSign() const { return sign; }

		bool operator==(_Token t) { 
			if(sign == t.GetSign() && value == t.GetValue() )	return true; 
			else return false;
		}

		bool operator==(_SignType s) { 
			if(sign == s)	return true; 
			else return false;
		}

		bool IsSign0() { //Ϊ��ʱ 
			return sign == _Token::DefSign;
		}

		bool IsDefined() { //�Ƿ��ж���
			return LEXICAL_ANALYSIS_DEFINE::IsTypeDefined(sign);
		}
	private:
		_SignType sign;
		_ValueType value;

	};//end of class

	typedef _Token<std::string,int> Token;

	template<class _SignType = int, class _DFAMachine = KH::DFARange>
	class TokenAnalysis {	//������Token�Ļ���,����һ��DFAMachine��һ����־sign
	public:
		typedef _SignType SignType;
		typedef _DFAMachine DFAMachine;

		TokenAnalysis() { ; }
		TokenAnalysis(_SignType st, _DFAMachine &d) {
			pDFAM = &d;
			sign = st;
		}

		//push
		void SetMachine(_DFAMachine &d) { pDFAM = &d; }
		void SetSign(_SignType st) { sign = st; }
		//pop
		_SignType GetSign() { return sign; }
		_DFAMachine* GetPMachine() { return pDFAM; }

		/*~TokenAnalysis(){
			free(pDFAM);
		}*/

	private:
		_DFAMachine* pDFAM; //!!!!!!!!!!!!
		_SignType sign;

	};//end of class

	_Token<> DefToken();

};//end of namespace 

#endif