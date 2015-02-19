#ifdef __cplusplus
extern "C" {
#endif

/* This file has been automatically generated by RPC-Generator
   https://github.com/Crystal-Photonics/RPC-Generator
   You should not modify this file manually. */


#include <stdint.h>
#include "RPC_client.h"

static const unsigned char *current;

static unsigned char expecting_answer;
/* =1 if a caller is waiting for an answer and 0 otherwise*/

RPC_RESULT square(int32_t return_value_out[1], int32_t i){
	/***Synchronizing***/
		RPC_mutex_lock(RPC_mutex_caller);
		RPC_mutex_lock(RPC_mutex_expected);
		expecting_answer = 1;
		RPC_mutex_unlock(RPC_mutex_expected);
		RPC_mutex_lock(RPC_mutex_sender);

	/***Serializing***/
		RPC_push_byte(2); /* save ID */

		/* writing integral type int32_t i of size 4 */
		RPC_push_byte(i);
		RPC_push_byte(i >> 8);
		RPC_push_byte(i >> 16);
		RPC_push_byte(i >> 24);

		RPC_mutex_unlock(RPC_mutex_sender);

	/***Communication***/
		RPC_commit();
		if (!(RPC_SLEEP()))
			return RPC_FAILURE;

	/***Deserializing***/
				
		/* reading integral type int32_t *return_value_out of size 4 */
		*return_value_out = current++;
		*return_value_out |= (*current++) << 8;
		*return_value_out |= (*current++) << 16L;
		*return_value_out |= (*current++) << 24L;

		return RPC_SUCCESS;
}


RPC_RESULT test(int32_t return_value_out[1], uint16_t data_inout[42]){
	/***Synchronizing***/
		RPC_mutex_lock(RPC_mutex_caller);
		RPC_mutex_lock(RPC_mutex_expected);
		expecting_answer = 1;
		RPC_mutex_unlock(RPC_mutex_expected);
		RPC_mutex_lock(RPC_mutex_sender);

	/***Serializing***/
		RPC_push_byte(4); /* save ID */

		/* writing array data_inout with 42 elements */
		{
			int RPC_COUNTER_VAR2;
			for (RPC_COUNTER_VAR2 = 0; RPC_COUNTER_VAR2 < 42; RPC_COUNTER_VAR2++){

				/* writing integral type uint16_t data_inout[RPC_COUNTER_VAR2] of size 2 */
				RPC_push_byte(data_inout[RPC_COUNTER_VAR2]);
				RPC_push_byte(data_inout[RPC_COUNTER_VAR2] >> 8);

			}
		}
		RPC_mutex_unlock(RPC_mutex_sender);

	/***Communication***/
		RPC_commit();
		if (!(RPC_SLEEP()))
			return RPC_FAILURE;

	/***Deserializing***/
				
		/* reading integral type int32_t *return_value_out of size 4 */
		*return_value_out = current++;
		*return_value_out |= (*current++) << 8;
		*return_value_out |= (*current++) << 16L;
		*return_value_out |= (*current++) << 24L;

		/* reading array data_inout with 42 elements */
		{
			int RPC_COUNTER_VAR2;
			for (RPC_COUNTER_VAR2 = 0; RPC_COUNTER_VAR2 < 42; RPC_COUNTER_VAR2++){

				/* reading integral type uint16_t data_inout[RPC_COUNTER_VAR2] of size 2 */
				data_inout[RPC_COUNTER_VAR2] = current++;
				data_inout[RPC_COUNTER_VAR2] |= (*current++) << 8;

			}
		}
		return RPC_SUCCESS;
}

/* Get (expected) size of (partial) message. */
RPC_SIZE_RESULT RPC_get_answer_length(const void *buffer, size_t size_bytes){
	RPC_SIZE_RESULT returnvalue = {RPC_SUCCESS, 0};
	const unsigned char *current = (const unsigned char *)buffer;
	if (!size_bytes){
		returnvalue.result = RPC_COMMAND_INCOMPLETE;
		returnvalue.size = 1;
		return returnvalue;
	}
	switch (*current){
		case 3: /* RPC_RESULT square(int32_t return_value_out[1], int32_t i); */
			returnvalue.size = 5;
			break;
		case 5: /* RPC_RESULT test(int32_t return_value_out[1], uint16_t data_inout[42]); */
			returnvalue.size = 89;
			break;
		default:
			returnvalue.result = RPC_COMMAND_UNKNOWN;
			return returnvalue;
	}
	if (returnvalue.size < size_bytes)
		returnvalue.result = RPC_COMMAND_INCOMPLETE;
	return returnvalue;
}

/* This function pushes the answers to the caller, doing all the necessary synchronization. */
RPC_SIZE_RESULT RPC_parse_answer(const void *buffer, size_t size_bytes){
	RPC_SIZE_RESULT returnvalue = RPC_get_answer_length(buffer, size_bytes);
	char expected = 1;
	if (returnvalue.result != RPC_SUCCESS)
		return returnvalue;
	current = (const unsigned char *)buffer;
	do{
		if (RPC_mutex_unlock(RPC_mutex_caller_pause)){ /* succeeded unpausing caller */
			RPC_mutex_lock(RPC_mutex_parser_pause); /* Pause parser, wait for caller to wake us up */
			return returnvalue; /* Successfully handed over answer to caller */
		}
		else{ /* failed unpausing caller */
			RPC_mutex_lock(RPC_mutex_expected);
			expected = expecting_answer; /* Is there still a caller waiting? */
			RPC_mutex_unlock(RPC_mutex_expected);
		}
	} while (expected);
	/* Got an invalid answer. Report as success for the network to discard the message. */
	return returnvalue;
}
#ifdef __cplusplus
}
#endif
