// This file is part of arduino-fsm.
//
// arduino-fsm is free software: you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// arduino-fsm is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with arduino-fsm.  If not, see <http://www.gnu.org/licenses/>.

#include "fsm.hpp"
#include "Arduino.h"

State::State(actionHandler_t on_en, actionHandler_t on_st, actionHandler_t on_ex)
: on_enter(on_en),
  on_state(on_st),
  on_exit(on_ex)
{
}


Fsm::Fsm(State* initial_state)
: m_current_state(initial_state),
  m_transitions(NULL),
  m_num_transitions(0),
  m_num_timed_transitions(0),
  m_initialized(false)
{
}


Fsm::~Fsm()
{
  free(m_transitions);
  free(m_timed_transitions);
  m_transitions = NULL;
  m_timed_transitions = NULL;
}


void Fsm::add_transition(State* state_from, State* state_to, int event,
                         void (*on_transition)())
{
  if (state_from == NULL || state_to == NULL)
    return;

  Transition transition = Fsm::create_transition(state_from, state_to, event,
                                               on_transition);
  m_transitions = (Transition*) realloc(m_transitions, (m_num_transitions + 1)
                                                       * sizeof(Transition));
  m_transitions[m_num_transitions] = transition;
#if 0
	Serial.println(F("-- ct --"));
	Serial.println(m_transitions[m_num_transitions].event);
	Serial.println((int)m_transitions[m_num_transitions].state_from);
#endif

  m_num_transitions++;
}


void Fsm::add_timed_transition(State* state_from, State* state_to,
                               unsigned long interval, void (*on_transition)())
{
  if (state_from == NULL || state_to == NULL)
    return;

  Transition transition = Fsm::create_transition(state_from, state_to, 0,
                                                 on_transition);

  TimedTransition timed_transition;
  timed_transition.transition = transition;
  timed_transition.start = 0;
  timed_transition.interval = interval;

  m_timed_transitions = (TimedTransition*) realloc(
      m_timed_transitions, (m_num_timed_transitions + 1) * sizeof(TimedTransition));
  m_timed_transitions[m_num_timed_transitions] = timed_transition;
  m_num_timed_transitions++;
}


Fsm::Transition Fsm::create_transition(State* state_from, State* state_to,
                                       int event, void (*on_transition)())
{
  Transition t;
  t.state_from = state_from;
  t.state_to = state_to;
  t.event = event;
  t.on_transition = on_transition;

  return t;
}

void Fsm::trigger(int event)
{

//	Serial.print(F("FSMT: ")); Serial.println(event);
#if 0
	Serial.println(m_num_transitions);
	Serial.println(m_initialized);
	Serial.println((int)m_current_state);
	Serial.println(event);
#endif
	if (m_initialized)
	{
		// Find the transition with the current state and given event.
		for (int i = 0; i < m_num_transitions; ++i)
		{
#if 0
			Serial.println("--");
			Serial.println(m_transitions[i].event);
			Serial.println((int)m_transitions[i].state_from);
#endif
			if (m_transitions[i].state_from == m_current_state
					&& m_transitions[i].event == event)
			{
				Fsm::make_transition(&(m_transitions[i]));
				return;
			}
		}
	}
}

void Fsm::check_timed_transitions()
{
#if TIMED_TRANSITION_ENABLED
  for (int i = 0; i < m_num_timed_transitions; ++i)
  {
    TimedTransition* transition = &m_timed_transitions[i];
    if (transition->transition.state_from == m_current_state)
    {
      if (transition->start == 0)
      {
        transition->start = millis();
      }
      else{
        unsigned long now = millis();
        if (now - transition->start >= transition->interval)
        {
          Fsm::make_transition(&(transition->transition));
          transition->start = 0;
        }
      }
    }
  }
#endif
}

void Fsm::run_machine()
{

  // first run must exec first state "on_enter"
  if (!m_initialized)
  {
    m_initialized = true;
    if (m_current_state->on_enter)
      m_current_state->on_enter();
  }

  if (m_current_state->on_state)
    m_current_state->on_state();
    
  Fsm::check_timed_transitions();
}

void Fsm::make_transition(Transition* transition)
{
 
  // Execute the handlers in the correct order.
  if (transition->state_from->on_exit)
    transition->state_from->on_exit();

  if (transition->on_transition != NULL)
    transition->on_transition();

  if (transition->state_to->on_enter)
    transition->state_to->on_enter();
  
  m_current_state = transition->state_to;

#if TIMED_TRANSITION_ENABLED
  //Initialice all timed transitions from m_current_state
  unsigned long now = millis();
  for (int i = 0; i < m_num_timed_transitions; ++i)
  {
    TimedTransition* ttransition = &m_timed_transitions[i];
    if (ttransition->transition.state_from == m_current_state)
      ttransition->start = now;
  }
#endif

}
