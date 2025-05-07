#pragma once

#include <grpc/grpc.h>
#include "Status.h"

namespace tweet {

	template<typename T>
	Status<T> StatusGrpcToStatusStorage(grpc::Status grpc_status)
	{
		switch (grpc_status.error_code())
		{
		case grpc::StatusCode::OK:
			return { StatusEnum::OK };
		case grpc::StatusCode::UNAUTHENTICATED:
			return {
				StatusEnum::UNAUTHENTICATED, grpc_status.error_message() };
		case grpc::StatusCode::INVALID_ARGUMENT:
			return {
				StatusEnum::INVALID_ARGUMENT, grpc_status.error_message() };
		case grpc::StatusCode::ALREADY_EXISTS:
			return {
				StatusEnum::ALREADY_EXISTS, grpc_status.error_message() };
		case grpc::StatusCode::PERMISSION_DENIED:
			return {
				StatusEnum::PERMISSION_DENIED, grpc_status.error_message() };
		default:
			return {
				StatusEnum::INTERNAL, grpc_status.error_message() };
		}
	}

	template<typename T>
	grpc::Status StatusStorageToStatusGrpc(
		const Status<T>& storage_status)
	{
		switch (storage_status.status)
		{
		case StatusEnum::OK:
			return grpc::Status::OK;
		case StatusEnum::UNAUTHENTICATED:
			return grpc::Status(
				grpc::StatusCode::UNAUTHENTICATED,
				storage_status.error_message);
		case StatusEnum::INVALID_ARGUMENT:
			return grpc::Status(
				grpc::StatusCode::INVALID_ARGUMENT,
				storage_status.error_message);
		case StatusEnum::ALREADY_EXISTS:
			return grpc::Status(
				grpc::StatusCode::ALREADY_EXISTS,
				storage_status.error_message);
		case StatusEnum::PERMISSION_DENIED:
			return grpc::Status(
				grpc::StatusCode::PERMISSION_DENIED,
				storage_status.error_message);
		default:
			return grpc::Status(
				grpc::StatusCode::INTERNAL,
				storage_status.error_message);
		}
	}

}
