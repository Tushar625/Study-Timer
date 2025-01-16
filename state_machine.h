#pragma once


/*
	how to use:

	Basic Usage:

	create an object of STATE_MACHINE class. Its constructor takes nothing.

	Example: STATE_MACHINE sm;

	now create the states, each state is a class inherited from BASE_STATE class ("public inheritance")
	with proper definition of Enter(), Loop(), Exit() functions, and create objects of those 'state classes'.
  These objects will serve as the states.

	Example:

	class initial_state : public BASE_STATE
	{
		void Enter()	// initialze this state
		{...}

		void Loop()	// working of this state
		{...}

		void Exit()		// destroy this state
		{...}
	} initial;

	inside the Loop() you call "sm.change_to()" to change the current state of the state machine
	this function takes a reference to a state object and makes it the current state. this function
	is also used to set the initial state of the state machine.

	note: a call to "sm.change_to()" from Loop() of a state must be last action of this function,
	as sm.change_to() calls the Exit() of current state i.e., destroys the current state

	Example:

	
  extern class play_state play;	// declaring the state object which is defined later

	//==========================
	// ~~~~~ initial state ~~~~~
	//==========================

	class initial_state : public BASE_STATE
	{
		...

		void Loop()
		{
			...
			sm.change_to(play);	// change state: 'initial' -> 'play' 
			...
		}

		...
	};
	
  initial_state initial;	// creating the state object

	
  //=======================
	// ~~~~~ play state ~~~~~
	//=======================
	
	class play_state : public BASE_STATE
	{
		...
	};
	
	play_state play;	// creating the state object
	

	void setup()
	{
		...
		sm.change_to(initial);	// setting 'intial' as initial state
		...
	}

	call sm.Loop() from loop(), this inturn calls the Loop() method
	of current state.

	By default this state machine stays at "NULL_STATE", an object of BASE_STATE
	class. STATE_MACHINE class has a method called "null_state()", that returns
	true when the state machine is at "NULL_STATE".

	To stop the state machine, just change the state to "NULL_STATE" and use
	"null_state()" method from the loop().

	Example:

	class initial_state : public BASE_STATE
	{
		...

		void Loop()
		{
			...

			if(...)
			{
				sm.change_to(NULL_STATE);	// change the state to "NULL_STATE"
			}
			
			...
		}

		...
	};

	...

	void loop()
	{
		...

		if(sm.end_state() == true)
		{
			// code to stop the system
			...
		}
		
		...
	}

	==================================================================================

	Local Data of states:

	You can simply have data members in state class, but if you want to have some data
	that will only exist while the state is in use, you should do it the following way,

	>>> Keep a structure in the class that represents or contains the data
	
	>>> Have a pointer to that structure as member data

	>>> Dynamically allocate the structure in Enter()

	>>> Destroy the structure in Exit()

	Example:

	class yoo_state : public BASE_STATE
	{
		
		// a structure nicely encaptulate the data used in this state locally


		struct data
		{
			// declare the variables i.e., data for the state, here

			...

			// initialize the variables with a constructor

			data(...)
			{...}

		};
		
		std::unique_ptr<data> p_data;


		// constructor of the state class
		
		yoo_state()
		{...}


		void Enter()
		{
			...
			// creating the local state data

			p_data = std::make_unique<data>(...);
			...
		}


		void Exit()
		{
			...
			// destroying the local state data

			b_data.reset();
			...
		}

	};

	==================================================================================

	Transferring data between states:

	An overloaded version of change_to() function can be used to send data while changing
	state, though the receiving state must have a function called "init()". see the example
	below,

	Example:


	extern class play_state play;	// declaring the state object which is defined later

	//==========================
	// ~~~~~ initial state ~~~~~
	//==========================

	class initial_state : public BASE_STATE
	{
		...

		public:

		// function to receive data

		void init(int _initial_data1, int _initial_data2)
		{
			initial_data1 = _initial_data1;

			initial_data2 = _initial_data2;
		}

		private:

		int initial_data1, initial_data2;

		...

		void Loop()
		{
			...
			// change state: 'initial' -> 'play' and sending data to play state

			sm.change_to(play, some_data);
			...
		}

		...
	};
	
	initial_state initial;	// creating the state object


	//=======================
	// ~~~~~ play state ~~~~~
	//=======================
	
	class play_state : public BASE_STATE
	{
		...
		
		public:

		// function to receive data

		void init(int _play_data)
		{
			play_data = _play_data;
		}

		private:

		int play_data;

		...
	};
	
	play_state play;	// creating the state object


	void setup()
	{
		...
		// setting 'intial' as initial state and sending data to it
		
		sm.change_to(initial, some_data1, some_data2);
		...
	}

*/


/*
	this is a template for states,

	states are actually classes that inherite this class and redefine the
	virtual methods as per their requirements
*/

class BASE_STATE
{
	public:

	virtual void Enter()
	{}

	virtual void Loop()
	{}

	virtual void Exit()
	{}
} NULL_STATE;


/*
	this is the actual state machine, it has a BASE_STATE pointer that points the
	current state
*/

class STATE_MACHINE
{
	/*
		points to the current states or points to NULL_STATE if no current state given
	*/
	
	BASE_STATE *current_state;


	public:

	STATE_MACHINE() : current_state(&NULL_STATE) // null state is set initially
	{}


	/*
		state transition
		exit from current state and set the next sate as its current state
	*/

	template<typename STATE_TYPE>
	
	void change_to(STATE_TYPE& next_state)
	{
		current_state->Exit();  // exit from current state

		current_state = &next_state;	// store address of this state

		current_state->Enter(); // enter new state
	}


  /*
    same as previous method but has ability to send some data to the new state
		by calling the init() of the new state
  */

	template<typename STATE_TYPE, typename ...type>
	
	void change_to(STATE_TYPE& next_state, type ...args)
	{
		current_state->Exit();  // exit from current state

		next_state.init(args...);

		current_state = &next_state;	// store address of this state

		current_state->Enter(); // enter new state
	}


	/*
		indicate when current_state points to NULL_STATE, i.e., this state
		machine has no active state
	*/

  bool null_state() const noexcept
	{
		return current_state == &NULL_STATE;
	}


	void Loop()
	{
		current_state->Loop();
	}
};