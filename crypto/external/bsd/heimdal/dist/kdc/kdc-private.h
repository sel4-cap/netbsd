/*	$NetBSD: kdc-private.h,v 1.2.10.1 2023/08/11 13:39:55 martin Exp $	*/

/* This is a generated file */
#ifndef __kdc_private_h__
#define __kdc_private_h__

#include <stdarg.h>

krb5_error_code
_kdc_add_KRB5SignedPath (
	krb5_context /*context*/,
	krb5_kdc_configuration */*config*/,
	hdb_entry_ex */*krbtgt*/,
	krb5_enctype /*enctype*/,
	krb5_const_principal /*client*/,
	krb5_const_principal /*server*/,
	krb5_principals /*principals*/,
	EncTicketPart */*tkt*/);

krb5_error_code
_kdc_add_inital_verified_cas (
	krb5_context /*context*/,
	krb5_kdc_configuration */*config*/,
	pk_client_params */*cp*/,
	EncTicketPart */*tkt*/);

krb5_error_code
_kdc_as_rep (
	kdc_request_t /*r*/,
	krb5_data */*reply*/,
	const char */*from*/,
	struct sockaddr */*from_addr*/,
	int /*datagram_reply*/);

krb5_error_code
_kdc_check_access (
	krb5_context /*context*/,
	krb5_kdc_configuration */*config*/,
	hdb_entry_ex */*client_ex*/,
	const char */*client_name*/,
	hdb_entry_ex */*server_ex*/,
	const char */*server_name*/,
	KDC_REQ */*req*/,
	METHOD_DATA */*method_data*/);

krb5_boolean
_kdc_check_addresses (
	krb5_context /*context*/,
	krb5_kdc_configuration */*config*/,
	HostAddresses */*addresses*/,
	const struct sockaddr */*from*/);

krb5_error_code
_kdc_check_anon_policy (
	krb5_context /*context*/,
	krb5_kdc_configuration */*config*/,
	hdb_entry_ex */*client*/,
	hdb_entry_ex */*server*/);

krb5_error_code
_kdc_db_fetch (
	krb5_context /*context*/,
	krb5_kdc_configuration */*config*/,
	krb5_const_principal /*principal*/,
	unsigned /*flags*/,
	krb5uint32 */*kvno_ptr*/,
	HDB **/*db*/,
	hdb_entry_ex **/*h*/);

krb5_error_code
_kdc_do_digest (
	krb5_context /*context*/,
	krb5_kdc_configuration */*config*/,
	const struct DigestREQ */*req*/,
	krb5_data */*reply*/,
	const char */*from*/,
	struct sockaddr */*addr*/);

krb5_error_code
_kdc_do_kx509 (
	krb5_context /*context*/,
	krb5_kdc_configuration */*config*/,
	const struct Kx509Request */*req*/,
	krb5_data */*reply*/,
	const char */*from*/,
	struct sockaddr */*addr*/);

krb5_error_code
_kdc_encode_reply (
	krb5_context /*context*/,
	krb5_kdc_configuration */*config*/,
	krb5_crypto /*armor_crypto*/,
	uint32_t /*nonce*/,
	KDC_REP */*rep*/,
	EncTicketPart */*et*/,
	EncKDCRepPart */*ek*/,
	krb5_enctype /*etype*/,
	int /*skvno*/,
	const EncryptionKey */*skey*/,
	int /*ckvno*/,
	const EncryptionKey */*reply_key*/,
	int /*rk_is_subkey*/,
	const char **/*e_text*/,
	krb5_data */*reply*/);

krb5_error_code
_kdc_fast_mk_error (
	krb5_context /*context*/,
	kdc_request_t /*r*/,
	METHOD_DATA */*error_method*/,
	krb5_crypto /*armor_crypto*/,
	const KDC_REQ_BODY */*req_body*/,
	krb5_error_code /*outer_error*/,
	const char */*e_text*/,
	krb5_principal /*error_server*/,
	const PrincipalName */*error_client_name*/,
	const Realm */*error_client_realm*/,
	time_t */*csec*/,
	int */*cusec*/,
	krb5_data */*error_msg*/);

krb5_error_code
_kdc_fast_mk_response (
	krb5_context /*context*/,
	krb5_crypto /*armor_crypto*/,
	METHOD_DATA */*pa_data*/,
	krb5_keyblock */*strengthen_key*/,
	KrbFastFinished */*finished*/,
	krb5uint32 /*nonce*/,
	krb5_data */*data*/);

krb5_error_code
_kdc_fast_unwrap_request (kdc_request_t /*r*/);

krb5_error_code
_kdc_find_etype (
	krb5_context /*context*/,
	krb5_boolean /*use_strongest_session_key*/,
	krb5_boolean /*is_preauth*/,
	hdb_entry_ex */*princ*/,
	krb5_enctype */*etypes*/,
	unsigned /*len*/,
	krb5_enctype */*ret_enctype*/,
	Key **/*ret_key*/);

const PA_DATA*
_kdc_find_padata (
	const KDC_REQ */*req*/,
	int */*start*/,
	int /*type*/);

void
_kdc_fix_time (time_t **/*t*/);

void
_kdc_free_ent (
	krb5_context /*context*/,
	hdb_entry_ex */*ent*/);

krb5_error_code
_kdc_generate_ecdh_keyblock (
	krb5_context /*context*/,
	void */*ec_key_pk*/,
	void **/*ec_key_key*/,
	unsigned char **/*dh_gen_key*/,
	size_t */*dh_gen_keylen*/);

krb5_error_code
_kdc_get_ecdh_param (
	krb5_context /*context*/,
	krb5_kdc_configuration */*config*/,
	SubjectPublicKeyInfo */*dh_key_info*/,
	void **/*out*/);

krb5_error_code
_kdc_get_preferred_key (
	krb5_context /*context*/,
	krb5_kdc_configuration */*config*/,
	hdb_entry_ex */*h*/,
	const char */*name*/,
	krb5_enctype */*enctype*/,
	Key **/*key*/);

krb5_boolean
_kdc_is_anon_request (const KDC_REQ */*req*/);

krb5_boolean
_kdc_is_anonymous (
	krb5_context /*context*/,
	krb5_const_principal /*principal*/);

krb5_boolean
_kdc_is_weak_exception (
	krb5_principal /*principal*/,
	krb5_enctype /*etype*/);

void
_kdc_log_timestamp (
	krb5_context /*context*/,
	krb5_kdc_configuration */*config*/,
	const char */*type*/,
	KerberosTime /*authtime*/,
	KerberosTime */*starttime*/,
	KerberosTime /*endtime*/,
	KerberosTime */*renew_till*/);

krb5_error_code
_kdc_make_anonymous_principalname (PrincipalName */*pn*/);

krb5_error_code
_kdc_pac_generate (
	krb5_context /*context*/,
	hdb_entry_ex */*client*/,
	krb5_pac */*pac*/);

krb5_error_code
_kdc_pac_verify (
	krb5_context /*context*/,
	const krb5_principal /*client_principal*/,
	const krb5_principal /*delegated_proxy_principal*/,
	hdb_entry_ex */*client*/,
	hdb_entry_ex */*server*/,
	hdb_entry_ex */*krbtgt*/,
	krb5_pac */*pac*/,
	int */*verified*/);

krb5_error_code
_kdc_pk_check_client (
	krb5_context /*context*/,
	krb5_kdc_configuration */*config*/,
	HDB */*clientdb*/,
	hdb_entry_ex */*client*/,
	pk_client_params */*cp*/,
	char **/*subject_name*/);

void
_kdc_pk_free_client_ec_param (
	krb5_context /*context*/,
	void */*ec_key_pk*/,
	void */*ec_key_key*/);

void
_kdc_pk_free_client_param (
	krb5_context /*context*/,
	pk_client_params */*cp*/);

krb5_error_code
_kdc_pk_mk_pa_reply (
	krb5_context /*context*/,
	krb5_kdc_configuration */*config*/,
	pk_client_params */*cp*/,
	const hdb_entry_ex */*client*/,
	krb5_enctype /*sessionetype*/,
	const KDC_REQ */*req*/,
	const krb5_data */*req_buffer*/,
	krb5_keyblock */*reply_key*/,
	krb5_keyblock */*sessionkey*/,
	METHOD_DATA */*md*/);

krb5_error_code
_kdc_pk_rd_padata (
	krb5_context /*context*/,
	krb5_kdc_configuration */*config*/,
	const KDC_REQ */*req*/,
	const PA_DATA */*pa*/,
	hdb_entry_ex */*client*/,
	pk_client_params **/*ret_params*/);

krb5_error_code
_kdc_serialize_ecdh_key (
	krb5_context /*context*/,
	void */*key*/,
	unsigned char **/*out*/,
	size_t */*out_len*/);

krb5_error_code
_kdc_tgs_rep (
	krb5_context /*context*/,
	krb5_kdc_configuration */*config*/,
	KDC_REQ */*req*/,
	krb5_data */*data*/,
	const char */*from*/,
	struct sockaddr */*from_addr*/,
	int /*datagram_reply*/);

krb5_error_code
_kdc_tkt_add_if_relevant_ad (
	krb5_context /*context*/,
	EncTicketPart */*tkt*/,
	int /*type*/,
	const krb5_data */*data*/);

krb5_error_code
_kdc_try_kx509_request (
	void */*ptr*/,
	size_t /*len*/,
	struct Kx509Request */*req*/,
	size_t */*size*/);

#endif /* __kdc_private_h__ */
