/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2017 jwellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

/*[[[cog
import cog
cog.outl("#if 0")
]]]*/
/*[[[end]]]*/
#error THIS HEADER IS A GENERATOR. DO NOT INCLUDE.
/*[[[cog
import cog
cog.outl("#endif")
]]]*/
/*[[[end]]]*/

/*[[[cog
import cog
cog.outl("//***************************************************************************")
cog.outl("// THIS FILE HAS BEEN AUTO GENERATED. DO NOT EDIT THIS FILE.")
cog.outl("//***************************************************************************")
]]]*/
/*[[[end]]]*/

//***************************************************************************
// To generate to header file, run this at the command line.
// Note: You will need Python and COG installed.
//
// python -m cogapp -d -e -ofsm.h -DHandlers=<n> fsm_generator.h
// Where <n> is the number of messages to support.
//
// e.g.
// To generate handlers for up to 16 events...
// python -m cogapp -d -e -ofsm.h -DHandlers=16 fsm_generator.h
//
// See generate.bat
//***************************************************************************

#ifndef ETL_FSM_INCLUDED
#define ETL_FSM_INCLUDED

#include <stdint.h>

#include "platform.h"
#include "array.h"
#include "nullptr.h"
#include "error_handler.h"
#include "exception.h"
#include "user_type.h"
#include "message_router.h"
#include "integral_limits.h"
#include "largest.h"

#undef ETL_FILE
#define ETL_FILE "34"

#include "private/minmax_push.h"

namespace etl
{
  class fsm;

  /// Allow alternative type for state id.
#if !defined(ETL_FSM_STATE_ID_TYPE)
    typedef uint_least8_t fsm_state_id_t;
#else
    typedef ETL_FSM_STATE_ID_TYPE fsm_state_id_t;
#endif

  // For internal FSM use.
  typedef typename etl::larger_type<etl::message_id_t>::type fsm_internal_id_t;

  //***************************************************************************
  /// Base exception class for FSM.
  //***************************************************************************
  class fsm_exception : public etl::exception
  {
  public:

    fsm_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : etl::exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Exception for null state pointer.
  //***************************************************************************
  class fsm_null_state_exception : public etl::fsm_exception
  {
  public:

    fsm_null_state_exception(string_type file_name_, numeric_type line_number_)
      : etl::fsm_exception(ETL_ERROR_TEXT("fsm:null state", ETL_FILE"A"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Exception for invalid state id.
  //***************************************************************************
  class fsm_state_id_exception : public etl::fsm_exception
  {
  public:

    fsm_state_id_exception(string_type file_name_, numeric_type line_number_)
      : etl::fsm_exception(ETL_ERROR_TEXT("fsm:state id", ETL_FILE"B"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Exception for incompatible state list.
  //***************************************************************************
  class fsm_state_list_exception : public etl::fsm_exception
  {
  public:

    fsm_state_list_exception(string_type file_name_, numeric_type line_number_)
      : etl::fsm_exception(ETL_ERROR_TEXT("fsm:state list", ETL_FILE"C"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Interface class for FSM states.
  //***************************************************************************
  class ifsm_state
  {
  public:

    /// Allows ifsm_state functions to be private.
    friend class etl::fsm;

    //*******************************************
    /// Gets the id for this state.
    //*******************************************
    etl::fsm_state_id_t get_state_id() const
    {
      return state_id;
    }

  protected:

    //*******************************************
    /// Constructor.
    //*******************************************
    ifsm_state(etl::fsm_state_id_t state_id_)
      : state_id(state_id_),
        p_context(ETL_NULLPTR)
    {
    }

    //*******************************************
    /// Destructor.
    //*******************************************
    ~ifsm_state()
    {
    }

    //*******************************************
    inline etl::fsm& get_fsm_context() const
    {
      return *p_context;
    }

  private:

    virtual fsm_state_id_t process_event(etl::imessage_router& source, const etl::imessage& message) = 0;

    virtual fsm_state_id_t on_enter_state() { return state_id; } // By default, do nothing.
    virtual void on_exit_state() {}  // By default, do nothing.

    //*******************************************
    void set_fsm_context(etl::fsm& context)
    {
      p_context = &context;
    }

    // The state id.
    const etl::fsm_state_id_t state_id;

    // A pointer to the FSM context.
    etl::fsm* p_context;

    // Disabled.
    ifsm_state(const ifsm_state&);
    ifsm_state& operator =(const ifsm_state&);
  };

  //***************************************************************************
  /// The FSM class.
  //***************************************************************************
  class fsm : public etl::imessage_router
  {
  public:

    //*******************************************
    /// Constructor.
    //*******************************************
    fsm(etl::message_router_id_t id)
      : imessage_router(id),
        p_state(ETL_NULLPTR)
    {
    }

    //*******************************************
    /// Set the states for the FSM
    //*******************************************
    template <typename TSize>
    void set_states(etl::ifsm_state** p_states, TSize size)
    {
      state_list       = p_states;
      number_of_states = etl::fsm_state_id_t(size);

      ETL_ASSERT((number_of_states > 0), ETL_ERROR(etl::fsm_state_list_exception));

      for (etl::fsm_state_id_t i = 0; i < size; ++i)
      {
        ETL_ASSERT((state_list[i] != ETL_NULLPTR), ETL_ERROR(etl::fsm_null_state_exception));
        state_list[i]->set_fsm_context(*this);
      }
    }

    //*******************************************
    /// Starts the FSM.
    /// Can only be called once.
    /// Subsequent calls will do nothing.
    ///\param call_on_enter_state If true will call on_enter_state() for the first state. Default = true.
    //*******************************************
    void start(bool call_on_enter_state = true)
    {
		  // Can only be started once.
		  if (p_state == ETL_NULLPTR)
		  {
			  p_state = state_list[0];
			  ETL_ASSERT(p_state != ETL_NULLPTR, ETL_ERROR(etl::fsm_null_state_exception));

			  if (call_on_enter_state)
			  {
				  etl::fsm_state_id_t next_state_id;
				  etl::ifsm_state*    p_last_state;

				  do
				  {
					  p_last_state = p_state;
					  next_state_id = p_state->on_enter_state();
					  p_state = state_list[next_state_id];

				  } while (p_last_state != p_state);
			  }
		  }
    }

    //*******************************************
    /// Top level message handler for the FSM.
    //*******************************************
    void receive(const etl::imessage& message)
    {
      static etl::null_message_router nmr;
      receive(nmr, message);
    }

    //*******************************************
    /// Top level message handler for the FSM.
    //*******************************************
    void receive(imessage_router& source, etl::message_router_id_t destination_router_id, const etl::imessage& message)
    {
      if ((destination_router_id == get_message_router_id()) || (destination_router_id == imessage_router::ALL_MESSAGE_ROUTERS))
      {
        receive(source, message);
      }
    }

    //*******************************************
    /// Top level message handler for the FSM.
    //*******************************************
    void receive(etl::imessage_router& source, const etl::imessage& message)
    {
        etl::fsm_state_id_t next_state_id = p_state->process_event(source, message);
        ETL_ASSERT(next_state_id < number_of_states, ETL_ERROR(etl::fsm_state_id_exception));

        etl::ifsm_state* p_next_state = state_list[next_state_id];

        // Have we changed state?
        if (p_next_state != p_state)
        {
          do
          {
            p_state->on_exit_state();
            p_state = p_next_state;

            next_state_id = p_state->on_enter_state();
            ETL_ASSERT(next_state_id < number_of_states, ETL_ERROR(etl::fsm_state_id_exception));

            p_next_state = state_list[next_state_id];

          } while (p_next_state != p_state); // Have we changed state again?
        }
    }

    using imessage_router::accepts;

    //*******************************************
    /// Does this FSM accept the message id?
    /// Yes, it accepts everything!
    //*******************************************
    bool accepts(etl::message_id_t) const
    {
      return true;
    }

    //*******************************************
    /// Gets the current state id.
    //*******************************************
    etl::fsm_state_id_t get_state_id() const
    {
      ETL_ASSERT(p_state != ETL_NULLPTR, ETL_ERROR(etl::fsm_null_state_exception));
      return p_state->get_state_id();
    }

    //*******************************************
    /// Gets a reference to the current state interface.
    //*******************************************
    ifsm_state& get_state()
    {
      ETL_ASSERT(p_state != ETL_NULLPTR, ETL_ERROR(etl::fsm_null_state_exception));
      return *p_state;
    }

    //*******************************************
    /// Gets a const reference to the current state interface.
    //*******************************************
    const ifsm_state& get_state() const
    {
      ETL_ASSERT(p_state != ETL_NULLPTR, ETL_ERROR(etl::fsm_null_state_exception));
      return *p_state;
    }

    //*******************************************
    /// Checks if the FSM has been started.
    //*******************************************
    bool is_started() const
    {
      return p_state != ETL_NULLPTR;
    }

    //*******************************************
    /// Reset the FSM to pre-started state.
    ///\param call_on_exit_state If true will call on_exit_state() for the current state. Default = false.
    //*******************************************
    void reset(bool call_on_exit_state = false)
    {
      if ((p_state != ETL_NULLPTR) && call_on_exit_state)
      {
        p_state->on_exit_state();
      }

      p_state = ETL_NULLPTR;
    }

    //********************************************
    ETL_DEPRECATED bool is_null_router() const ETL_OVERRIDE
    {
      return false;
    }

    //********************************************
    bool is_producer() const ETL_OVERRIDE
    {
      return true;
    }

    //********************************************
    bool is_consumer() const ETL_OVERRIDE
    {
      return false;
    }

  private:

    etl::ifsm_state*    p_state;          ///< A pointer to the current state.
    etl::ifsm_state**   state_list;       ///< The list of added states.
    etl::fsm_state_id_t number_of_states; ///< The number of states.
  };

  /*[[[cog
  import cog
  ################################################
  # The first definition for all of the events.
  ################################################
  cog.outl("//***************************************************************************")
  cog.outl("// The definition for all %s message types." % Handlers)
  cog.outl("//***************************************************************************")
  cog.outl("template <typename TContext, typename TDerived, const etl::fsm_state_id_t STATE_ID_, ")
  cog.out("          ")
  for n in range(1, int(Handlers)):
      cog.out("typename T%s = void, " % n)
      if n % 4 == 0:
          cog.outl("")
          cog.out("          ")
  cog.outl("typename T%s = void>" % Handlers)
  cog.outl("class fsm_state : public ifsm_state")
  cog.outl("{")
  cog.outl("public:")
  cog.outl("")
  cog.outl("  enum")
  cog.outl("  {")
  cog.outl("    STATE_ID = STATE_ID_")
  cog.outl("  };")
  cog.outl("")
  cog.outl("  fsm_state()")
  cog.outl("    : ifsm_state(STATE_ID)")
  cog.outl("  {")
  cog.outl("  }")
  cog.outl("")
  cog.outl("protected:")
  cog.outl("")
  cog.outl("  ~fsm_state()")
  cog.outl("  {")
  cog.outl("  }")
  cog.outl("")
  cog.outl("  inline TContext& get_fsm_context() const")
  cog.outl("  {")
  cog.outl("    return static_cast<TContext&>(ifsm_state::get_fsm_context());")
  cog.outl("  }")
  cog.outl("")
  cog.outl("private:")
  cog.outl("")
  cog.outl("  etl::fsm_state_id_t process_event(etl::imessage_router& source, const etl::imessage& message)")
  cog.outl("  {")
  cog.outl("    etl::fsm_state_id_t new_state_id;")
  cog.outl("    etl::message_id_t event_id = message.message_id;")
  cog.outl("")
  cog.outl("    switch (event_id)")
  cog.outl("    {")
  for n in range(1, int(Handlers) + 1):
      cog.out("      case T%d::ID:" % n)
      cog.out(" new_state_id = static_cast<TDerived*>(this)->on_event(source, static_cast<const T%d&>(message));" % n)
      cog.outl(" break;")
  cog.out("      default:")
  cog.out(" new_state_id = static_cast<TDerived*>(this)->on_event_unknown(source, message);")
  cog.outl(" break;")
  cog.outl("    }")
  cog.outl("")
  cog.outl("    return new_state_id;")
  cog.outl("  }")
  cog.outl("};")

  ####################################
  # All of the other specialisations.
  ####################################
  for n in range(int(Handlers) - 1, 0, -1):
      cog.outl("")
      cog.outl("//***************************************************************************")
      if n == 1:
          cog.outl("// Specialisation for %d message type." % n)
      else:
          cog.outl("// Specialisation for %d message types." % n)
      cog.outl("//***************************************************************************")
      cog.outl("template <typename TContext, typename TDerived, const etl::fsm_state_id_t STATE_ID_, ")
      cog.out("          ")
      for t in range(1, n):
          cog.out("typename T%d, " % t)
          if t % 4 == 0:
              cog.outl("")
              cog.out("          ")
      cog.outl("typename T%d>" % n)
      cog.out("class fsm_state<TContext, TDerived, STATE_ID_, ")
      for t in range(1, n + 1):
          cog.out("T%d, " % t)
      if t % 16 == 0:
          cog.outl("")
          cog.out("               ")
      for t in range(n + 1, int(Handlers)):
          cog.out("void, ")
      if t % 16 == 0:
          cog.outl("")
          cog.out("               ")
      cog.outl("void> : public ifsm_state")
      cog.outl("{")
      cog.outl("public:")
      cog.outl("")
      cog.outl("  enum")
      cog.outl("  {")
      cog.outl("    STATE_ID = STATE_ID_")
      cog.outl("  };")
      cog.outl("")
      cog.outl("  fsm_state()")
      cog.outl("    : ifsm_state(STATE_ID)")
      cog.outl("  {")
      cog.outl("  }")
      cog.outl("")
      cog.outl("protected:")
      cog.outl("")
      cog.outl("  ~fsm_state()")
      cog.outl("  {")
      cog.outl("  }")
      cog.outl("")
      cog.outl("  inline TContext& get_fsm_context() const")
      cog.outl("  {")
      cog.outl("    return static_cast<TContext&>(ifsm_state::get_fsm_context());")
      cog.outl("  }")
      cog.outl("")
      cog.outl("private:")
      cog.outl("")
      cog.outl("  etl::fsm_state_id_t process_event(etl::imessage_router& source, const etl::imessage& message)")
      cog.outl("  {")
      cog.outl("    etl::fsm_state_id_t new_state_id;")
      cog.outl("    etl::message_id_t event_id = message.message_id;")
      cog.outl("")
      cog.outl("    switch (event_id)")
      cog.outl("    {")
      for n in range(1, n + 1):
          cog.out("      case T%d::ID:" % n)
          cog.out(" new_state_id = static_cast<TDerived*>(this)->on_event(source, static_cast<const T%d&>(message));" % n)
          cog.outl(" break;")
      cog.out("      default:")
      cog.out(" new_state_id = static_cast<TDerived*>(this)->on_event_unknown(source, message);")
      cog.outl(" break;")
      cog.outl("    }")
      cog.outl("")
      cog.outl("    return new_state_id;")
      cog.outl("  }")
      cog.outl("};")
  ####################################
  # Specialisation for zero messages.
  ####################################
  cog.outl("")
  cog.outl("//***************************************************************************")
  cog.outl("// Specialisation for 0 message types.")
  cog.outl("//***************************************************************************")
  cog.outl("template <typename TContext, typename TDerived, const etl::fsm_state_id_t STATE_ID_>")
  cog.out("class fsm_state<TContext, TDerived, STATE_ID_, ")
  for t in range(1, int(Handlers)):
      cog.out("void, ")
  if t % 16 == 0:
      cog.outl("")
      cog.out("               ")
  cog.outl("void> : public ifsm_state")
  cog.outl("{")
  cog.outl("public:")
  cog.outl("")
  cog.outl("  enum")
  cog.outl("  {")
  cog.outl("    STATE_ID = STATE_ID_")
  cog.outl("  };")
  cog.outl("")
  cog.outl("  fsm_state()")
  cog.outl("    : ifsm_state(STATE_ID)")
  cog.outl("  {")
  cog.outl("  }")
  cog.outl("")
  cog.outl("protected:")
  cog.outl("")
  cog.outl("  ~fsm_state()")
  cog.outl("  {")
  cog.outl("  }")
  cog.outl("")
  cog.outl("  inline TContext& get_fsm_context() const")
  cog.outl("  {")
  cog.outl("    return static_cast<TContext&>(ifsm_state::get_fsm_context());")
  cog.outl("  }")
  cog.outl("private:")
  cog.outl("")
  cog.outl("  etl::fsm_state_id_t process_event(etl::imessage_router& source, const etl::imessage& message)")
  cog.outl("  {")
  cog.outl("    return static_cast<TDerived*>(this)->on_event_unknown(source, message);")
  cog.outl("  }")
  cog.outl("};")
  ]]]*/
  /*[[[end]]]*/
}

#undef ETL_FILE

#include "private/minmax_pop.h"

#endif
