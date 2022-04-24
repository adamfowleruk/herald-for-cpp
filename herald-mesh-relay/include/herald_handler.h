/*
 * Copyright 2022 Herald Project Contributors
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Provides application-level coordination for the Herald Bluetooth 
 * LE and Herald Presence Server functionality
 */

#ifndef HERALD_HANDLER_H__
#define HERALD_HANDLER_H__

#ifdef __cplusplus
extern "C"
{
#endif

/** \brief One off Herald initialisation once beacon is configured **/
void herald_initialise();

void herald_healthcheck();

bool herald_configure();

bool herald_start();

bool herald_stop();

#ifdef __cplusplus
}
#endif

#endif /* HERALD_HANDLER_H__ */
