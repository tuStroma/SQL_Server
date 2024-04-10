#pragma once

enum SQL_context {
	Login,
	LoginSuccess,
	LoginFail,

	Register,
	RegisterSuccess,
	RegisterFail,

	SendTable,

	SelectEmployees
};