#pragma once

/// std
#include <optional>
#include <string>

/// engine
#include "../IAsset.h"
#include "Engine/Core/DirectX12/Resource/DxResource.h"



namespace ONEngine::Asset {
template<typename T>
class AssetLoader;
}

namespace ONEngine {
class DxDevice;
class DxSRVHeap;
class DxCommand;
}



namespace ONEngine::Asset {


/**
 * @enum TextureFormat
 * @brief テクスチャデータ形式の列挙型
 */
enum class TextureFormat {
	RGBA8_UNORM,  ///< 8ビット符号なし正規化整数 RGBA
	RGBA8_SRGB,   ///< 8ビット符号なし正規化整数 RGBA（sRGB色空間）
	RGBA16_FLOAT, ///< 16ビット浮動小数点数 RGBA
	RGBA32_FLOAT, ///< 32ビット浮動小数点数 RGBA
	RGBA32_UINT,  ///< 32ビット符号なし整数 RGBA
};

/**
 * @enum ColorSpace
 * @brief 使用する色空間（ガンマ）の列挙型
 */
enum class ColorSpace {
	Linear, ///< リニア空間
	sRGB    ///< sRGB（ガンマ）空間
};

/**
 * @brief jsonオブジェクトからTextureFormat列挙型へのデシリアライズを行います。
 */
void from_json(const nlohmann::json& j, TextureFormat& format);
/**
 * @brief TextureFormat列挙型からjsonオブジェクトへのシリアライズを行います。
 */
void to_json(nlohmann::json& j, const TextureFormat& format);

/**
 * @brief jsonオブジェクトからColorSpace列挙型へのデシリアライズを行います。
 */
void from_json(const nlohmann::json& j, ColorSpace& colorSpace);
/**
 * @brief ColorSpace列挙型からjsonオブジェクトへのシリアライズを行います。
 */
void to_json(nlohmann::json& j, const ColorSpace& colorSpace);

/**
 * @class Texture
 * @brief 画像データ、レンダリングターゲット、UAV用の書き込みバッファ等を管理するテクスチャアセットクラス
 */
class Texture final : public IAsset {
	friend class AssetLoader<Texture>;
public:
	/// ===================================================
	/// public : sub class
	/// ===================================================

	/**
	 * @struct MetaData
	 * @brief テクスチャアセット固有のメタデータ（フォーマットや色空間）
	 */
	struct MetaData {
		TextureFormat format;     ///< テクスチャフォーマット
		ColorSpace colorSpace;   ///< 色空間
	};

	/**
	 * @struct Handle
	 * @brief ディスクリプタヒープ内のハンドル情報
	 */
	struct Handle {
		uint32_t descriptorIndex;              ///< ヒープインデックス
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle; ///< CPUハンドル
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle; ///< GPUハンドル
	};

public:
	/// ===================================================
	/// public : methods
	/// ===================================================

	/**
	 * @brief デフォルトコンストラクタ
	 */
	Texture();
	/**
	 * @brief サイズを指定したコンストラクタ
	 * @param _textureSize 初期サイズ
	 */
	Texture(const Vector2& _textureSize);
	/**
	 * @brief デストラクタ
	 */
	~Texture() override = default;

	/**
	 * @brief 空のSRVハンドル情報を作成・アロケートします。
	 */
	void CreateEmptySRVHandle();

	/**
	 * @brief 空のUAVハンドル情報を作成・アロケートします。
	 */
	void CreateEmptyUAVHandle();


	/**
	 * @brief 書き込み可能なレンダリングリソース（UAV）としてテクスチャを作成します。
	 * @param _width テクスチャの幅
	 * @param _height テクスチャの高さ
	 * @param _dxDevice グラフィックスデバイス
	 * @param _dxSRVHeap SRV用のディスクリプタヒープ
	 * @param _dxgiFormat DXGIフォーマット
	 */
	void CreateUAVTexture(UINT _width, UINT _height, DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap, DXGI_FORMAT _dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT);

	/**
	 * @brief 3DのUAVテクスチャを作成します。
	 * @param _width 幅
	 * @param _height 高さ
	 * @param _depth 奥行き
	 * @param _dxDevice デバイス
	 * @param _dxSRVHeap ディスクリプタヒープ
	 * @param _dxgiFormat DXGIフォーマット
	 */
	void CreateUAVTexture3DWithUAV(UINT _width, UINT _height, UINT _depth, DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap, DXGI_FORMAT _dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT);

	/**
	 * @brief 既存の3DテクスチャにUAVビューを追加します。
	 * @param _width 幅
	 * @param _height 高さ
	 * @param _depth 奥行き
	 * @param _dxDevice デバイス
	 * @param _dxSRVHeap ディスクリプタヒープ
	 * @param _dxgiFormat DXGIフォーマット
	 */
	void CreateUAVTexture3D(UINT _width, UINT _height, UINT _depth, DxDevice* _dxDevice, DxSRVHeap* _dxSRVHeap, DXGI_FORMAT _dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT);


	/**
	 * @brief テクスチャをファイル（PNGなど）に非同期出力します。
	 * @param _filename ファイル名
	 * @param _dxDevice デバイス
	 * @param _dxCommand コマンドリスト
	 */
	void OutputTexture(const std::wstring& _filename, DxDevice* _dxDevice, DxCommand* _dxCommand);
	/**
	 * @brief 3Dテクスチャをファイル（DDS）に出力します。
	 * @param _filename ファイル名
	 * @param _dxDevice デバイス
	 * @param _dxCommand コマンドリスト
	 */
	void OutputTexture3D(const std::wstring& _filename, DxDevice* _dxDevice, DxCommand* _dxCommand);

	/**
	 * @brief 3Dテクスチャのサイズをリサイズし再生成します。
	 * @param _newSize 新しい2D解像度
	 * @param _newDepth 新しい奥行き
	 * @param _dxDevice デバイス
	 * @param _dxCommand コマンドリスト
	 * @param _dxSRVHeap ディスクリプタヒープ
	 */
	void ResizeTexture3D(const Vector2& _newSize, UINT _newDepth, DxDevice* _dxDevice, DxCommand* _dxCommand, DxSRVHeap* _dxSRVHeap);

private:
	/// ===================================================
	/// private : objects
	/// ===================================================

	std::string name_;

	DxResource dxResource_;
	DxResource readbackTexture_;

	std::optional<Handle> srvHandle_;
	std::optional<Handle> uavHandle_;

	Vector2 textureSize_;
	UINT depth_ = 0; // 3Dテクスチャ用
	UINT arraySize_ = 1; // 配列テクスチャ用
	bool isCubeMap_ = false;

	/// テクスチャのフォーマット、UAVを作成する際に必要
	DXGI_FORMAT srvFormat_;
	DXGI_FORMAT uavFormat_;

public:
	/// ===================================================
	/// public : accessor
	/// ===================================================

	/// ----- setters ----- ///

	/**
	 * @brief テクスチャのデバッグ識別名を設定します。
	 * @param _name 識別名
	 */
	void SetName(const std::string& _name);

	/**
	 * @brief SRV用のディスクリプタ情報をセットします。
	 */
	void SetSRVHandle(const Handle& _handle);
	/**
	 * @brief UAV用のディスクリプタ情報をセットします。
	 */
	void SetUAVHandle(const Handle& _handle);

	/**
	 * @brief SRVディスクリプタ情報を個別にセットします。
	 */
	void SetSRVHandle(uint32_t _descriptorIndex, D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle);
	/**
	 * @brief UAVディスクリプタ情報を個別にセットします。
	 */
	void SetUAVHandle(uint32_t _descriptorIndex, D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle);

	/**
	 * @brief SRV用ディスクリプタインデックスをセットします。
	 */
	void SetSRVDescriptorIndex(uint32_t _index);
	/**
	 * @brief SRV用CPUディスクリプタハンドルをセットします。
	 */
	void SetSRVCPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle);
	/**
	 * @brief SRV用GPUディスクリプタハンドルをセットします。
	 */
	void SetSRVGPUHandle(D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle);

	/**
	 * @brief UAV用ディスクリプタインデックスをセットします。
	 */
	void SetUAVDescriptorIndex(uint32_t _index);
	/**
	 * @brief UAV用CPUディスクリプタハンドルをセットします。
	 */
	void SetUAVCPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle);
	/**
	 * @brief UAV用GPUディスクリプタハンドルをセットします。
	 */
	void SetUAVGPUHandle(D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle);


	/// ----- getters ----- ///

	/**
	 * @brief SRV用のディスクリプタハンドル情報を取得します。
	 */
	const Handle& GetSRVHandle() const;
	/**
	 * @brief UAV用のディスクリプタハンドル情報を取得します。
	 */
	const Handle& GetUAVHandle() const;

	/**
	 * @brief SRV用のディスクリプタインデックスを取得します。
	 */
	uint32_t GetSRVDescriptorIndex() const;
	/**
	 * @brief SRV用のCPUディスクリプタハンドルを取得します。
	 */
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUHandle() const;
	/**
	 * @brief SRV用のGPUディスクリプタハンドルを取得します。
	 */
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle() const;
	/**
	 * @brief SRVハンドル情報が正常にアロケートされているかを取得します。
	 */
	bool HasSRVHandle() const;

	/**
	 * @brief UAV用のディスクリプタインデックスを取得します。
	 */
	uint32_t GetUAVDescriptorIndex() const;
	/**
	 * @brief UAV用のCPUディスクリプタハンドルを取得します。
	 */
	D3D12_CPU_DESCRIPTOR_HANDLE GetUAVCPUHandle() const;
	/**
	 * @brief UAV用のGPUディスクリプタハンドルを取得します。
	 */
	D3D12_GPU_DESCRIPTOR_HANDLE GetUAVGPUHandle() const;
	/**
	 * @brief UAVハンドル情報が正常にアロケートされているかを取得します。
	 */
	bool HasUAVHandle() const;

	/**
	 * @brief テクスチャのDX12リソースオブジェクト（読み取り専用）を取得します。
	 */
	const DxResource& GetDxResource() const;
	/**
	 * @brief テクスチャのDX12リソースオブジェクトを取得します。
	 */
	DxResource& GetDxResource();

	/**
	 * @brief テクスチャの2Dピクセル解像度を取得します。
	 */
	const Vector2& GetTextureSize() const;
	/**
	 * @brief 3Dテクスチャの奥行き（デプス）を取得します。
	 */
	UINT GetTextureDepth() const;
	/**
	 * @brief このテクスチャがキューブマップかどうかを取得します。
	 */
	bool IsCubeMap() const { return isCubeMap_; }
	/**
	 * @brief このテクスチャが標準的な2Dシングルテクスチャ（配列・ボリューム・キューブ以外）であるかどうかを取得します。
	 */
	bool IsStandard2D() const { return !isCubeMap_ && depth_ == 0 && arraySize_ == 1; }

};


/**
 * @brief CPU側の画素バッファデータをPNG画像ファイルに書き出します。
 * @param _filename 保存先のファイルパス
 * @param _width 幅
 * @param _height 高さ
 * @param _overwrite 上書き保存するかどうか
 */
void SaveTextureToPNG(const std::wstring& _filename, size_t _width, size_t _height, bool _overwrite);

/**
 * @brief CPU側の画素データをDDS形式のテクスチャファイルに書き出します（3D/ボリューム用）。
 * @param _filename 保存先のファイルパス
 * @param _width 幅
 * @param _height 高さ
 * @param _depth 奥行き
 * @param _overwrite 上書きするかどうか
 */
void SaveTextureToDDS(const std::wstring& _filename, size_t _width, size_t _height, size_t _depth, bool _overwrite);

} /// ONEngine
