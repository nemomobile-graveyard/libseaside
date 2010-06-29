/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "seasidedetail.h"

Q_IMPLEMENT_CUSTOM_CONTACT_DETAIL(SeasideCustomDetail, "Seaside");
Q_DEFINE_LATIN1_CONSTANT(SeasideCustomDetail::FieldFavorite, "Favorite");
Q_DEFINE_LATIN1_CONSTANT(SeasideCustomDetail::FieldCommTimestamp, "CommTimestamp");
Q_DEFINE_LATIN1_CONSTANT(SeasideCustomDetail::FieldCommType, "CommType");
Q_DEFINE_LATIN1_CONSTANT(SeasideCustomDetail::FieldCommLocation, "CommLocation");
