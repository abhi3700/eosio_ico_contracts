// #pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
// #include <eosio/print.hpp>
#include <eosio/system.hpp>
#include <eosio/crypto.hpp>
#include <string>

using eosio::contract;
using eosio::print;
using eosio::name;
using eosio::multi_index;
using eosio::const_mem_fun;
using eosio::indexed_by;
using eosio::asset;
using eosio::check;
using eosio::permission_level;
using eosio::datastream;
using eosio::current_time_point;
using eosio::action;
using eosio::same_payer;
using eosio::symbol;
using eosio::require_recipient;
using eosio::checksum256;
using eosio::action_wrapper;

using std::string;


CONTRACT dciico : public contract
{
private:
	const symbol ride_token_symbol;
	const name token_contract_ac;

public:
	using contract::contract;

	dciico(name receiver, name code, datastream<const char*> ds) : 
				contract(receiver, code, ds), 
				fund_token_symbol("EOS", 4),
				token_contract_ac("dci1111token"_n) {}


	/**
	 * @brief - deposit fund to the contract
	 * @details 
	 * 		- deposit the `amount` to the contract to recieve tokens at ICO rate.
	 * 		- this table `fund` acts as a record-keeper for amounts sent by the buyers
	 * @param buyer_ac - buyer account
	 * @param contract_ac - contract account
	 * @param quantity - the deposit amount
	 * @param memo - remarks
	 */
	[[eosio::on_notify("dci1111token::transfer")]]
	void deposit( const name& buyer_ac, 
					const name& contract_ac, 
					const asset& quantity, 
					const string& memo );

	/**
	 * @brief - set ICO rate
	 * @details - set ICO rate
	 * 
	 * @param phase - A / B / C
	 * @param price_pereos - price per EOS token
	 * 
	 */
	ACTION seticorate( const name& phase,
						float price_pereos );

	/**
	 * @brief - only accessed by contract
	 * @details - disburse from fund to 
	 * 
	 * @param receiver_ac - account to whom money is disbursed
	 * @param quantity - disburse amount
	 * @param memo - purpose of disbursing money
	 */
	ACTION disburse( const name& receiver_ac,
					const asset& quantity,
					const string& memo );

	/**
	 * @brief - send alert
	 * @details - send alert after the action is successfully done. e.g. after the disburse action is implemented
	 * 
	 * @param user - driver/commuter
	 * @param message - note depending on the action
	 */
	ACTION sendalert( const name& user,
						const string& message);



	using disburse_action  = action_wrapper<"disburse"_n, &dciico::disburse>;

	static void check_quantity( const asset& quantity ) {
		check(quantity.is_valid(), "invalid quantity");
		check(quantity.amount > 0, "must withdraw positive quantity");
		check(quantity.symbol == symbol("EOS", 4), "symbol precision mismatch. Also, could be bcoz of sending some other tokens to this contract.");
	}

private:
	// `fund` table is for keeping the record of all deposits in EOS
	TABLE fund
	{
		asset balance;

		auto primary_key() const { return balance.symbol.raw(); }
	};

	using fund_index = multi_index<"fund"_n, fund>;

	// -----------------------------------------------------------------------------------------------------------------------
	TABLE icorate
	{
		name phase;
		float price_pereos;		// 1 EOS = 30.56 DCI. So, price_pereos = 30.56
	};

	using icorate_index = multi_index<"icorates", icorate>;

	// -----------------------------------------------------------------------------------------------------------------------
	// Adding inline action for `sendalert` action in the same contract 
	void send_alert(const name& user, const string& message);

};


