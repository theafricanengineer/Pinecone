#pragma once

#include "Token.h"
#include "Action.h"
#include "Namespace.h"

#include <vector>
using std::vector;

class AstNodeBase;

typedef shared_ptr<AstNodeBase> AstNode;

AstNode astNodeFromTokens(const vector<Token>&);

class AstNodeBase
{
public:
	
	void setInput(Namespace nsIn, Type left, Type right)
	{
		if (ns || inLeftType || inRightType)
		{
			throw PineconeError("tried to set input on an AST node more then once", INTERNAL_ERROR);
		}
		
		ns=nsIn;
		inLeftType=left;
		inRightType=right;
	}
	
	virtual string getString()=0;
	
	Type getReturnType(Namespace ns)
	{
		if (!returnType)
		{
			if (!ns)
			{
				throw PineconeError("tried to get return type from AST node when input had not been set", INTERNAL_ERROR);
			}
			
			resolveReturnType();
		}
		
		return returnType;
	}
	
	Action getAction()
	{
		if (!action)
		{
			if (!ns)
			{
				throw PineconeError("tried to get action from AST node when input had not been set", INTERNAL_ERROR);
			}
			
			resolveAction();
		}
		
		return action;
	}
	
protected:
	
	virtual void resolveReturnType()
	{
		returnType=getAction()->getReturnType();
	}
	
	virtual void resolveAction()=0;
	
	Type inLeftType=nullptr, inRightType=nullptr;
	Action action=nullptr;
	Type returnType=nullptr;
	Namespace ns=nullptr;
};

class AstVoid: public AstNodeBase
{
public:
	
	static shared_ptr<AstVoid> make() {return shared_ptr<AstVoid>(new AstVoid);}
	
	string getString() {return "void node";}
	void resolveReturnType() {returnType=Void;}
	void resolveAction() {action=voidAction;}
};

extern AstNode astVoid;

class AstList: public AstNodeBase
{
public:
	
	//	make a new instance of this type of node
	static shared_ptr<AstList> make(const vector<AstNode>& in)
	{
		shared_ptr<AstList> node(new AstList);
		node->nodes=in;
		return node;
	}
	
	string getString();
	
	void resolveReturnType();
	
	void resolveAction();
	
private:
	
	//	the list of sub nodes
	vector<AstNode> nodes;
};

class AstExpression: public AstNodeBase
{
public:
	
	static shared_ptr<AstExpression> make(AstNode leftInIn, AstNode centerIn, AstNode rightInIn)
	{
		if (leftInIn==astVoid && rightInIn==astVoid)
		{
			throw PineconeError("tried to make an AstExpression node with both inputs void", INTERNAL_ERROR);
		}
		
		shared_ptr<AstExpression> node(new AstExpression);
		
		node->leftIn=leftInIn;
		node->center=centerIn;
		node->rightIn=rightInIn;
		
		return node;
	}
	
	string getString();
	
	void resolveAction();
	
private:
	
	AstExpression() {}
	
	AstNode leftIn=nullptr, center=nullptr, rightIn=nullptr;
};


class AstToken: public AstNodeBase
{
public:
	
	static shared_ptr<AstToken> make(Token tokenIn)
	{
		shared_ptr<AstToken> node(new AstToken);
		
		node->token=tokenIn;
		
		return node;
	}
	
	string getString();
	
	void resolveAction();
	
private:
	
	AstToken() {}
	
	Token token=nullptr;
};

