/*
 * Copyright (C) 2017 - 2019 National Institute of Advanced Industrial Science
 *                           and Technology (AIST)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <tee_client_api.h>

/* TEEC Stub */

/**
 * TEEC_InitializeContext() - Initializes a context holding connection
 * information on the specific TEE, designated by the name string.
 *
 * @param name			A zero-terminated string identifying the TEE to connect to.
 * 				If name is set to NULL, the default TEE is connected to. NULL
 *				is the only supported value in this version of the API
 *				implementation.
 *
 * @param context		The context structure which is to be initialized.
 *
 * @return TEEC_SUCCESS		The initialization was successful.
 * @return TEEC_Result		Something failed.
 */
TEEC_Result TEEC_InitializeContext(const char *name, TEEC_Context *context)
{
//	pr_deb("%s: stub called\n", __func__);
	return TEEC_SUCCESS;
}

/**
 * TEEC_FinalizeContext() - Destroys a context holding connection information
 * on the specific TEE.
 *
 * This function finalizes an initialized TEE context, closing the connection
 * between the client application and the TEE. This function must only be
 * called when all sessions related to this TEE context have been closed and
 * all shared memory blocks have been released.
 *
 * @param context		The context to be finalized.
 */
void TEEC_FinalizeContext(TEEC_Context *context)
{
//	pr_deb("%s: stub called\n", __func__);
}

/**
 * TEEC_OpenSession() - Opens a new session with the specified trusted
 * application.
 *
 * @param context		The initialized TEE context structure in which
 * 				scope to open the session.
 * @param session		The session to initialize.
 * @param destination		A structure identifying the trusted application
 * 				with which to open a session.
 *
 * @param connectionMethod	The connection method to use.
 * @param connectionData	Any data necessary to connect with the chosen
 * 				connection method. Not supported, should be set to
 * 				NULL.
 * @param operation		An operation structure to use in the session. May
 * 				be set to NULL to signify no operation structure
 * 				needed.
 *
 * @param returnOrigin		A parameter which will hold the error origin if
 * 				this function returns any value other than
 *				TEEC_SUCCESS.
 *
 * @return TEEC_SUCCESS		OpenSession successfully opened a new session.
 * @return TEEC_Result		Something failed.
 *
 */
TEEC_Result TEEC_OpenSession(TEEC_Context *context,
		TEEC_Session *session,
		const TEEC_UUID *destination,
		uint32_t connectionMethod,
		const void *connectionData,
		TEEC_Operation *operation,
		uint32_t *returnOrigin)
{
//	pr_deb("%s: stub called\n", __func__);
	return TEEC_SUCCESS;
}

/**
 * TEEC_CloseSession() - Closes the session which has been opened with the
 * specific trusted application.
 *
 * @param session The opened session to close.
 */
void TEEC_CloseSession(TEEC_Session *session)
{
//	pr_deb("%s: stub called\n", __func__);
}

/**
 * TEEC_RegisterSharedMemory() - Register a block of existing memory as a
 * shared block within the scope of the specified context.
 *
 * @param context			The initialized TEE context structure in which scope to
 * 					open the session.
 * @param sharedMem			pointer to the shared memory structure to register.
 *
 * @return TEEC_SUCCESS			The registration was successful.
 * @return TEEC_ERROR_OUT_OF_MEMORY	Memory exhaustion.
 * @return TEEC_Result			Something failed.
 */
TEEC_Result TEEC_RegisterSharedMemory(TEEC_Context *context,
		TEEC_SharedMemory *sharedMem)
{
//	pr_deb("%s: stub called\n", __func__);
	return TEEC_SUCCESS;
}

/**
 * TEEC_AllocateSharedMemory() - Allocate shared memory for TEE.
 *
 * @param context			The initialized TEE context structure in which scope to
 * 					open the session.
 * @param sharedMem			Pointer to the allocated shared memory.
 *
 * @return TEEC_SUCCESS			The registration was successful.
 * @return TEEC_ERROR_OUT_OF_MEMORY	Memory exhaustion.
 * @return TEEC_Result			Something failed.
 */
TEEC_Result TEEC_AllocateSharedMemory(TEEC_Context *context,
		TEEC_SharedMemory *sharedMem)
{
//	pr_deb("%s: stub called\n", __func__);
	return TEEC_SUCCESS;
}

/**
 * TEEC_ReleaseSharedMemory() - Free or deregister the shared memory.
 *
 * @param sharedMem	Pointer to the shared memory to be freed.
 */
void TEEC_ReleaseSharedMemory(TEEC_SharedMemory *sharedMemory)
{
//	pr_deb("%s: stub called\n", __func__);
}

/**
 * TEEC_RequestCancellation() - Request the cancellation of a pending open
 *                              session or command invocation.
 *
 * @param operation	Pointer to an operation previously passed to open session
 * 			or invoke.
 */
void TEEC_RequestCancellation(TEEC_Operation *operation)
{
//	pr_deb("%s: stub called\n", __func__);
}
